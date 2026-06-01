/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    pimpleFoam.C

Group
    grpIncompressibleSolvers

Description
    Transient solver for incompressible, turbulent flow of Newtonian fluids
    on a moving mesh.

    \heading Solver details
    The solver uses the PIMPLE (merged PISO-SIMPLE) algorithm to solve the
    continuity equation:

        \f[
            \div \vec{U} = 0
        \f]

    and momentum equation:

        \f[
            \ddt{\vec{U}} + \div \left( \vec{U} \vec{U} \right) - \div \gvec{R}
          = - \grad p + \vec{S}_U
        \f]

    Where:
    \vartable
        \vec{U} | Velocity
        p       | Pressure
        \vec{R} | Stress tensor
        \vec{S}_U | Momentum source
    \endvartable

    Sub-models include:
    - turbulence modelling, i.e. laminar, RAS or LES
    - run-time selectable MRF and finite volume options, e.g. explicit porosity

    \heading Required fields
    \plaintable
        U       | Velocity [m/s]
        p       | Kinematic pressure, p/rho [m2/s2]
        \<turbulence fields\> | As required by user selection
    \endplaintable

Note
   The motion frequency of this solver can be influenced by the presence
   of "updateControl" and "updateInterval" in the dynamicMeshDict.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "singlePhaseTransportModel.H"
#include "turbulentTransportModel.H"
#include "pimpleControl.H"
#include "CorrectPhi.H"
#include "fvOptions.H"
#include "localEulerDdtScheme.H"
#include "fvcSmooth.H"

// In-situ visualization
#define IN_SITU_VIS
#if defined( IN_SITU_VIS )
//#include "InSituVis.h"
#include "InSituVis.h"
#include <InSituVis/Lib.foam/FoamToKVS.h>


//#define IN_SITU_VIS__P
//#define IN_SITU_VIS__U
#define IN_SITU_VIS__Q
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Transient solver for incompressible, turbulent flow"
        " of Newtonian fluids on a moving mesh."
    );

    #include "postProcess.H"

    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createDynamicFvMesh.H"
    #include "initContinuityErrs.H"
    #include "createDyMControls.H"
    #include "createFields.H"
    #include "createUfIfPresent.H"
    #include "CourantNo.H"
    #include "setInitialDeltaT.H"

    turbulence->validate();

    if (!LTS)
    {
        #include "CourantNo.H"
        #include "setInitialDeltaT.H"
    }

#if defined( IN_SITU_VIS )
    // In-situ visualization setup
    Foam::messageStream::level = 0; // Disable Foam::Info
    const kvs::Indent indent(4); // indent for log stream
    local::InSituVis vis( MPI_COMM_WORLD );
    if ( !vis.initialize() )
      {
        vis.log() << "ERROR: " << "Cannot initialize visualization process." << std::endl;
        vis.world().abort();
      }

    // Time-loop information
    const auto start_time = runTime.startTime().value();
    const auto start_time_index = runTime.startTimeIndex();
    const auto end_time = runTime.endTime().value();
    const auto end_time_index = static_cast<int>( end_time / runTime.deltaT().value() );
    vis.log() << std::endl;
    vis.log() << "STARTING TIME LOOP" << std::endl;
    vis.log() << indent << "Start time and index: " << start_time << ", " << start_time_index << std::endl;
    vis.log() << indent << "End time and index: " << end_time << ", " << end_time_index << std::endl;
    vis.log() << std::endl;
#endif // IN_SITU_VIS
    
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
        #include "readDyMControls.H"

        if (LTS)
        {
            #include "setRDeltaT.H"
        }
        else
        {
            #include "CourantNo.H"
            #include "setDeltaT.H"
        }

        ++runTime;

#if defined( IN_SITU_VIS )
        // Loop information
        const auto current_time_value = runTime.value();
        const auto current_time_index = runTime.timeIndex();
        vis.log() << "LOOP[" << current_time_index << "/" << end_time_index << "]: " << std::endl;
        vis.log() << indent << "T: " << current_time_value << std::endl;
        vis.log() << indent << "End T: " << end_time << std::endl;
        vis.log() << indent << "Delta T: " << runTime.deltaT().value() << std::endl;
        vis.simTimer().start();
#endif // IN_SITU_VIS
        

        Info<< "Time = " << runTime.timeName() << nl << endl;

        // --- Pressure-velocity PIMPLE corrector loop
        while (pimple.loop())
        {
            if (pimple.firstIter() || moveMeshOuterCorrectors)
            {
                // Do any mesh changes
                mesh.controlledUpdate();

                if (mesh.changing())
                {
                    MRF.update();

                    if (correctPhi)
                    {
                        // Calculate absolute flux
                        // from the mapped surface velocity
                        phi = mesh.Sf() & Uf();

                        #include "correctPhi.H"

                        // Make the flux relative to the mesh motion
                        fvc::makeRelative(phi, U);
                    }

                    if (checkMeshCourantNo)
                    {
                        #include "meshCourantNo.H"
                    }
                }
            }

            #include "UEqn.H"

            // --- Pressure corrector loop
            while (pimple.correct())
            {
                #include "pEqn.H"
            }

            if (pimple.turbCorr())
            {
                laminarTransport.correct();
                turbulence->correct();
            }
        }

        runTime.write();

#if defined( IN_SITU_VIS )
        vis.simTimer().stamp();
        const auto ts = vis.simTimer().last();
        const auto Ts = kvs::String::From( ts, 4 );
        vis.log() << indent << "Processing Times:" << std::endl;
        vis.log() << indent.nextIndent() << "Simulation: " << Ts << " s" << std::endl;
#if defined( IN_SITU_VIS__P ) // p: pressure
        auto& field = p;
        const auto min_value = 0.999990 * 100000.0;
        const auto max_value = 1.000200 * 100000.0;
#elif defined( IN_SITU_VIS__U ) // U: velocity
        auto& field = U;
        const auto min_value = 0.0;
        const auto max_value = 71.645393372;
#elif defined( IN_SITU_VIS__Q )
        volTensorField gradU = fvc::grad(U);
        volScalarField Q
          (
           IOobject("Q", runTime.timeName(), mesh, IOobject::NO_READ, IOobject::NO_WRITE),
           0.5 * (magSqr(skew(gradU)) - magSqr(symm(gradU)))
           );

        auto& field = Q;

        const double act_min = gMin(field);
        const double act_max = gMax(field);

        const auto min_value = 1000;
        const auto max_value = 10000;        
#endif

        // Convert OpenFOAM data to KVS data
        vis.cnvTimer().start();
        InSituVis::foam::FoamToKVS converter( field );
        using CellType = InSituVis::foam::FoamToKVS::CellType;
        auto vol_tet = converter.exec( vis.world(), field, CellType::Tetrahedra );
        auto vol_hex = converter.exec( vis.world(), field, CellType::Hexahedra );
        auto vol_pri = converter.exec( vis.world(), field, CellType::Prism );
        auto vol_pyr = converter.exec( vis.world(), field, CellType::Pyramid );
        vis.cnvTimer().stamp();

        vol_tet.setName("Tet");
        vol_hex.setName("Hex");
        vol_pri.setName("Pri");
        vol_pyr.setName("Pyr");

        vol_tet.setMinMaxValues( min_value, max_value );
        vol_hex.setMinMaxValues( min_value, max_value );
        vol_pri.setMinMaxValues( min_value, max_value );
        vol_pyr.setMinMaxValues( min_value, max_value );

        const auto tc = vis.cnvTimer().last();
        const auto Tc = kvs::String::From( tc, 4 );
        vis.log() << indent.nextIndent() << "Conversion: " << Tc << " s" << std::endl;

        // Execute visualization pipeline and rendering
        vis.visTimer().start();
        vis.put( vol_tet );
        vis.put( vol_hex );
        vis.put( vol_pri );
        vis.put( vol_pyr );
        vis.exec( { current_time_value, current_time_index } );
        vis.visTimer().stamp();

        const auto tv = vis.visTimer().last();
        const auto Tv = kvs::String::From( tv, 4 );
        vis.log() << indent.nextIndent() << "Visualization: " << Tv << " s" << std::endl;

        const auto elapsed_time = runTime.elapsedCpuTime();
        vis.log() << indent << "Elapsed Time: " << elapsed_time << " s" << std::endl;
        vis.log() << std::endl;
#endif // IN_SITU_VIS        
        
        runTime.printExecutionTime(Info);
    }

#if defined( IN_SITU_VIS )
    if ( !vis.finalize() )
    {
        vis.log() << "ERROR: " << "Cannot finalize visualization process." << std::endl;
        vis.world().abort();
    }
#endif // IN_SITU_VIS

    
    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
