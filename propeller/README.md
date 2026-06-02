# propellerの解析

ここではOpenFOAM v2412のtutorialに含まれるpropellerの非定常解析のin-situ可視化を行います．propellerの回転によって誘起されるQ値の等値面を可視化します．


## 準備と実行コマンド

以下の操作を実行します．まずはソースコードを取得します．
```
$ cd ~/Work/GitHub
$ git clone https://github.com/vizlab-kobe/OpenFOAMVis.git
```

ソルバーディレクトリに移動してビルドをします．デフォルトではEGL版がビルドされます．
```
$ cd OpenFOAMVis/propeller/propeller_pimpleFoam
$ wclean && wmake
```
OSMesa版をビルドしたい場合には
```
$ cd OpenFOAMVis/propeller/propeller_pimpleFoam
$ cp Make/option_osmesa Make/option
$ wclean && wmake
```
として下さい．以上の操作でin-situ可視化に対応したpropeller解析用の非定常非圧縮ソルバー`propeller_pimpleFoam`がビルドされます．

さらにpropeller解析ディレクトリに移動して解析を実行します．
```
$ cd ~/Work/GitHub/OpenFOAMVis/propeller/vis_propeller
$ decomposePar
$ mpirun np 4 propeller_pimpleFoam -parallel
```
として実行します．環境にも依りますが，計算に非常に長い時間がかかりますので，
```
$ nohup mpirun -np 4 propeller_pimpleFoam -parallel 2>&1 log.dat &
```
として
```
$ tail -f log.dat
```
で監視しても良いです．`Output`ディレクトリに`*.bmp`ファイルが出力されればOKです．




## 編集の詳細（参考情報）
### 解析の準備
まずは解析の準備を整えます．

```
$ mkdir ~/local/calc
$ cp -r $FOAM_TUTORIALS/incompressible/pimpleFoam/RAS/propeller ~/local/calc/vis_propeller
```

※なお，`vis_propeller`ディレクトリには**大量の画像（および解析結果ファイル）が生成**されることになります．
余裕のある記憶媒体かどうか確認することをおすすめします．

坂本研究室の関係者で，研究室PC`10.34.33.219`を使用している方は，
```
$ cp -r $FOAM_TUTORIALS/incompressible/pimpleFoam/RAS/propeller /data2/(user_name)/local/calc/vis_propeller
```
として下さい．

次にメッシュの準備をします．
```
$ cd ~/local/calc/vis_propeller
$ ./Allrun.pre
```
完了後，`stl`を準備します．
```
$ cd ./constant/triSurface
```
ここには`*.obj.gz`ファイルが並んでいます．今回使用するのは回転するプロペラ部分の3Dオブジェクトである`propellerTip.obj.gz`です．展開した後，`surfaceConvert`というOpenFOAMのコマンドを使用して`obj`から`stl`に変換します．
```
$ gunzip propellerTip.obj.gz
$ surfaceConvert propellerTip.obj propellerTip.stl
```

残りの準備も済ませておきます．
```
$ cd ../../
$ cp -r 0.org 0
$ decomposePar
```

## ソルバーの編集
ソルバーの改造環境を作成します．
```
$ cd ~/local
$ mkdir OpenFOAM
$ cd OpenFOAM
$ cp -r $FOAM_SOLVERS/incompressible/pimpleFoam/ ./propeller_pimpleFoam
$ cd propeller_pimpleFoam
```

まずは`Make/files`を以下のとおり編集します．
```diff
-EXE = $(FOAM_APPBIN)/pimpleFoam
+EXE = $(FOAM_USER_APPBIN)/propeller_pimpleFoam
```
この設定を行うことで，今までのpimpleFoam（非定常非圧縮流体向けソルバー）を維持したままpropeller解析用の特別な設定を施したソルバーpropeller_pimpleFoamが使用可能になります．

次に`Make/options`を以下のように編集します．EGL版の場合は
```diff
EXE_INC = \
    -I$(LIB_SRC)/finiteVolume/lnInclude \
    -I$(LIB_SRC)/meshTools/lnInclude \
    -I$(LIB_SRC)/sampling/lnInclude \
    -I$(LIB_SRC)/TurbulenceModels/turbulenceModels/lnInclude \
    -I$(LIB_SRC)/TurbulenceModels/incompressible/lnInclude \
    -I$(LIB_SRC)/transportModels \
    -I$(LIB_SRC)/transportModels/incompressible/singlePhaseTransportModel \
    -I$(LIB_SRC)/dynamicMesh/lnInclude \
    -I$(LIB_SRC)/dynamicFvMesh/lnInclude \
    -I$(LIB_SRC)/regionFaModels/lnInclude \
+    -I$(HOME)/local/openmpi-4.1.2/include \

EXE_LIBS = \
    -lfiniteVolume \
    -lfvOptions \
    -lmeshTools \
    -lsampling \
    -lturbulenceModels \
    -lincompressibleTurbulenceModels \
    -lincompressibleTransportModels \
    -ldynamicMesh \
    -ldynamicFvMesh \
    -ltopoChangerFvMesh \
    -latmosphericModels \
    -lregionFaModels \
    -lfiniteArea

+/* KVS settings (EGL / GPU Mode) */
+EXE_INC += \
+    -I${KVS_DIR}/include -DKVS_SUPPORT_MPI -DKVS_USE_MPI \
+    -DKVS_SUPPORT_EGL -DEGL_NO_X11 -DMESA_EGL_NO_X11_HEADERS

+EXE_LIBS += \
+    -L${KVS_DIR}/lib -lkvsSupportMPI \
+    -lkvsSupportEGL -lkvsCore \
+    -lEGL -lGL

+/* InSitu settings */
+EXE_INC += -I$(HOME)/Work/GitHub
+EXE_LIBS += -L$(HOME)/Work/GitHub/InSituVis/Lib -lInSituVis

+/* OpenMP settings */
+EXE_INC += -fopenmp
+EXE_LIBS += -fopenmp
```
です．

osmesa版の場合には
```diff
EXE_INC = \
    -I$(LIB_SRC)/finiteVolume/lnInclude \
    -I$(LIB_SRC)/meshTools/lnInclude \
    -I$(LIB_SRC)/sampling/lnInclude \
    -I$(LIB_SRC)/TurbulenceModels/turbulenceModels/lnInclude \
    -I$(LIB_SRC)/TurbulenceModels/incompressible/lnInclude \
    -I$(LIB_SRC)/transportModels \
    -I$(LIB_SRC)/transportModels/incompressible/singlePhaseTransportModel \
    -I$(LIB_SRC)/dynamicMesh/lnInclude \
    -I$(LIB_SRC)/dynamicFvMesh/lnInclude \
    -I$(LIB_SRC)/regionFaModels/lnInclude \
+    -I$(HOME)/local/openmpi-4.1.2/include \

EXE_LIBS = \
    -lfiniteVolume \
    -lfvOptions \
    -lmeshTools \
    -lsampling \
    -lturbulenceModels \
    -lincompressibleTurbulenceModels \
    -lincompressibleTransportModels \
    -ldynamicMesh \
    -ldynamicFvMesh \
    -ltopoChangerFvMesh \
    -latmosphericModels \
    -lregionFaModels \
    -lfiniteArea

+LLVM_LIB = $(HOME)/local/llvm_15.0.7/lib

+/* KVS settings */
+EXE_INC += \
+        -I${KVS_DIR}/include -DKVS_SUPPORT_MPI -DKVS_USE_MPI\
+        -I${KVS_OSMESA_DIR}/include -DKVS_SUPPORT_OSMESA
+EXE_LIBS += \
+    -L$(LLVM_LIB) \
+        -L${KVS_DIR}/lib -lkvsSupportMPI -lkvsSupportOSMesa -lkvsCore \
+        -L${KVS_OSMESA_DIR}/lib/x86_64-linux-gnu ${KVS_OSMESA_LINK_LIBRARY} \
+        -L$(KVS_LIB_DIR) -lkvs

+/* InSitu settings */
+EXE_INC += -I$(HOME)/Work/Github
+EXE_LIBS += -L$(HOME)/Work/Github/InSituVis/Lib -lInSituVis

+/* OpenMP settings */
+EXE_INC += -fopenmp
+EXE_LIBS += -fopenmp
```
となります．

次のソルバーの改造です．OralAirFlowVisとほとんど同じですが，Q値の等値面可視化用のプラグインを入れます．

OralAirFlowVisと同様，5つの改造箇所があります．
1つ目：ヘッダーの部分
```cpp
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
```

2つ目と3つ目はOralAirFlowVisと同じ．

4つ目．時間発展のwhile文の内部です．Q値の可視化を行います．
```cpp
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
```
5つ目もOralAirFlowVisと同じ．

`InSituVis.h`をコピーします．
```
cp ~/Work/GitHub/OralAirFlowVis/rhoPimpleFoam_InSituVis/InSituVis.h ~/local/OpenFOAM/propeller_pimpleFoam
```

`InSituVis.h`を編集します．

まず冒頭の部分ですが
```diff
// Parameters
//----------------------------------------------------------------------------
namespace Params
{
struct Output
{
static const auto Image = true;
static const auto SubImage = false;
static const auto SubImageDepth = false;
static const auto SubImageAlpha = false;
};

const auto ImageSize = kvs::Vec2ui{ 512, 512 }; // width x height
const auto AnalysisInterval = 5; // l: analysis (visuaization) time interval

- const auto VisibleBoundingBox = true;
- const auto VisibleBoundaryMesh = false;
+ const auto VisibleBoundingBox = false;
+ const auto VisibleBoundaryMesh = true;

// For IN_SITU_VIS__VIEWPOINT__*
const auto ViewPos = kvs::Vec3{ 4, 3, 4.5 }; // viewpoint position
const auto ViewDim = kvs::Vec3ui{ 3, 3, 3 }; // viewpoint dimension
const auto ViewDir = InSituVis::Viewpoint::Direction::Uni; // Uni or Omni
const auto Viewpoint = InSituVis::Viewpoint{ { ViewDir, ViewPos } };
const auto ViewpointCubic = InSituVis::CubicViewpoint{ ViewDim, ViewDir };
const auto ViewpointSpherical = InSituVis::SphericalViewpoint{ ViewDim, ViewDir };

// For IN_SITU_VIS__ADAPTOR__STOCHASTIC_RENDERING
const auto Repeats = 50; // number of repetitions for stochastic rendering
const auto BoundaryMeshOpacity = 30; // opacity value [0-255] of boundary mesh
}

/*****************************************************************************/



namespace local
{

class InSituVis : public ::Adaptor
{
    using BaseClass = ::Adaptor;
    using Object = BaseClass::Object;
    using Volume = kvs::UnstructuredVolumeObject;
    using Screen = BaseClass::Screen;

public:
    static Pipeline WholeMinMaxValues();
    static Pipeline OrthoSlice();
    static Pipeline Isosurface();
    static Pipeline ExternalFace( const kvs::mpi::Communicator& world );
    static Pipeline StochasticRendering( const size_t repeats );

private:
    kvs::PolygonObject m_boundary_mesh; ///< boundary mesh
    kvs::mpi::StampTimer m_sim_timer{ BaseClass::world() }; ///< timer for sim. process
    kvs::mpi::StampTimer m_cnv_timer{ BaseClass::world() }; ///< timer for cnv. process
    kvs::mpi::StampTimer m_vis_timer{ BaseClass::world() }; ///< timer for vis. process
    kvs::Real64 m_whole_min_value = 0.0; ///< min. value of whole time-varying volume data
    kvs::Real64 m_whole_max_value = 0.0; ///< max. value of whole time-varying volume data

public:
    InSituVis( const MPI_Comm world = MPI_COMM_WORLD, const int root = 0 ): BaseClass( world, root )
    {

+      this->setOutputFilename("propeller");

        // Common parameters.
        this->setImageSize( Params::ImageSize.x(), Params::ImageSize.y() );
        this->setOutputImageEnabled( Params::Output::Image );
        this->setOutputSubImageEnabled(
            Params::Output::SubImage,
            Params::Output::SubImageDepth,
            Params::Output::SubImageAlpha );

        // Import boundary mesh.
-        this->importBoundaryMesh( "./constant/triSurface/realistic-cfd3.stl" );
+        this->importBoundaryMesh("./constant/triSurface/propellerTip.stl");
        // Time intervals.
        this->setAnalysisInterval( Params::AnalysisInterval );

        // Set visualization pipeline.
#if defined( IN_SITU_VIS__ADAPTOR__STOCHASTIC_RENDERING )
        this->setRepetitionLevel( Params::Repeats );
        this->setPipeline( local::InSituVis::StochasticRendering( Params::Repeats ) );
#elif defined( IN_SITU_VIS__PIPELINE__ORTHO_SLICE )
        this->setPipeline( local::InSituVis::OrthoSlice() );
#elif defined( IN_SITU_VIS__PIPELINE__ISOSURFACE )
        this->setPipeline( local::InSituVis::Isosurface() );
#elif defined( IN_SITU_VIS__PIPELINE__EXTERNAL_FACE )
        this->setPipeline( local::InSituVis::ExternalFace( BaseClass::world() ) );
#endif
```
さらにその下で，`stl`ファイルの可視化と回転を行います．既存の設定のではIsosurface可視化の際に`stl`が出現しません．
```diff
    void exec( const BaseClass::SimTime sim_time )
    {
        if ( !BaseClass::screen().scene()->hasObject( "BoundaryMesh") )
        {
            const bool visible = BaseClass::world().rank() == BaseClass::world().root ();
            auto* object = new kvs::PolygonObject();
            object->shallowCopy( m_boundary_mesh );
            object->setName( "BoundaryMesh" );
            object->setVisible( visible );

            // Register the bounding box at the root rank.
#if defined( IN_SITU_VIS__ADAPTOR__STOCHASTIC_RENDERING )
            // Bounding box
            kvs::Bounds bounds( kvs::RGBColor::Black(), 1.0f );
            auto* o = bounds.outputLineObject( object );
            o->setVisible( object->isVisible() );
            auto* r = new kvs::StochasticLineRenderer();
            BaseClass::screen().registerObject( o, r );
            // Boundary mesh
            object->setOpacity( Params::BoundaryMeshOpacity );
            auto* renderer = new kvs::StochasticPolygonRenderer();
            renderer->setTwoSideLightingEnabled( true );
            renderer->setEdgeFactor( 0.6f );
            BaseClass::screen().registerObject( object, renderer );
#else

            // Bounding box
-            BaseClass::screen().registerObject( object, new kvs::Bounds() );
-            object->setVisible( Params::VisibleBoundingBox );

+            object->setVisible( visible );
+            if ( Params::VisibleBoundingBox )
+              {
+                BaseClass::screen().registerObject( object, new kvs::Bounds() );
+              }

+            if ( Params::VisibleBoundaryMesh )
+              {
+                auto* renderer = new kvs::PolygonRenderer();
+                renderer->setTwoSideLightingEnabled( true );
+                BaseClass::screen().registerObject( object, renderer );
+              }
#endif
        }

+        auto* mesh = kvs::PolygonObject::DownCast( BaseClass::screen().scene()->object( "BoundaryMesh" ) );
+        if ( mesh )
+          {
+            const double omega = 158.0;          // omega [rad/s]
+            const kvs::Vec3 axis( 0, 1, 0 );     // axis (y-axis)
+            double angle_deg = ( omega * sim_time.value ) * ( 180.0 / kvs::Math::PI() );
+            kvs::Matrix33f R = kvs::Matrix33f::Rotation( axis, angle_deg );
+            kvs::Xform xform = mesh->xform();
+            kvs::Xform new_xform( xform.translation(), xform.scaling(), R );
+            mesh->setXform( new_xform );
+          }

#if defined( IN_SITU_VIS__UPDATE_MIN_MAX_VALUES )
        // Update min/max values of the volume data in each time step.
        auto min_value = Volume::DownCast( *BaseClass::objects().begin() )->minValue();
```
デフォルトのtutorialではy軸周りに158 [rad/s]で回転します．設定は`~/local/calc/vis_propeller/constant/dynamicMeshDict`で確認できます．

scaleの設定を改めます．importBoundaryMesh関数について
```diff
    void importBoundaryMesh( const std::string& filename )
    {
        m_boundary_mesh = kvs::PolygonImporter( filename );

        // Scaling coordinate values of the boundary object adjusing to
        // the coordinate scale of the volume dataset.
-       const auto scale = 1.0f / 1000.0f;
+       const auto scale = 1.0f;
        {
            auto coords = m_boundary_mesh.coords();
            for ( auto& p : coords ) { p *= scale; }
            m_boundary_mesh.setCoords( coords );
        }

        const auto min_coord = m_boundary_mesh.minObjectCoord() * scale;
        const auto max_coord = m_boundary_mesh.maxObjectCoord() * scale;
        m_boundary_mesh.setMinMaxObjectCoords( min_coord, max_coord );
        m_boundary_mesh.setMinMaxExternalCoords( min_coord, max_coord );

```
この設定を行うことで，`stl`ファイルと解析可視化が等倍で可視化できるようになります．

解析領域全体の表示，およびboundary meshの二重登録回避のために
```diff
inline InSituVis::Pipeline InSituVis::Isosurface()
{
    return [&] ( Screen& screen, const Object& object )
    {
        Volume volume; volume.shallowCopy( Volume::DownCast( object ) );
        if ( volume.numberOfCells() == 0 ) { return; }

        auto* mesh = kvs::PolygonObject::DownCast( screen.scene()->object( "BoundaryMesh" ) );
        if ( mesh )
        {
-            const auto min_coord = mesh->minExternalCoord();
-            const auto max_coord = mesh->maxExternalCoord();
-            volume.setMinMaxObjectCoords( min_coord, max_coord );
-            volume.setMinMaxExternalCoords( min_coord, max_coord );
        }

        // Setup a transfer function.
        const auto min_value = volume.minValue();
        const auto max_value = volume.maxValue();
        //auto t = kvs::TransferFunction( kvs::ColorMap::CoolWarm() );
        auto t = kvs::TransferFunction( kvs::ColorMap::BrewerSpectral() );
        t.setRange( min_value, max_value );
        // Create new object
        auto n = kvs::Isosurface::PolygonNormal;
        auto d = true;
        auto i0 = kvs::Math::Mix( min_value, max_value, 0.1 );
        auto* object0 = new kvs::Isosurface( &volume, i0, n, d, t );
        object0->setName( volume.name() + "Object0");

        auto i1 = kvs::Math::Mix( min_value, max_value, 0.3 );
        auto* object1 = new kvs::Isosurface( &volume, i1, n, d, t );
        object1->setName( volume.name() + "Object1");

        auto i2 = kvs::Math::Mix( min_value, max_value, 0.7 );
        auto* object2 = new kvs::Isosurface( &volume, i2, n, d, t );
        object2->setName( volume.name() + "Object2");

        // Register object and renderer to screen
        kvs::Light::SetModelTwoSide( true );
        if ( screen.scene()->hasObject( volume.name() + "Object0") )
        {
            // Update the objects.
            screen.scene()->replaceObject( volume.name() + "Object0", object0 );
            screen.scene()->replaceObject( volume.name() + "Object1", object1 );
            screen.scene()->replaceObject( volume.name() + "Object2", object2 );
        }
        else
        {
            // Register the objects with renderer.
            auto* renderer0 = new kvs::glsl::PolygonRenderer();
            auto* renderer1 = new kvs::glsl::PolygonRenderer();
            auto* renderer2 = new kvs::glsl::PolygonRenderer();
            renderer0->setTwoSideLightingEnabled( true );
            renderer1->setTwoSideLightingEnabled( true );
            renderer2->setTwoSideLightingEnabled( true );
            screen.registerObject( object0, renderer0 );
            screen.registerObject( object1, renderer1 );
            screen.registerObject( object2, renderer2 );

            // Boundary mesh
-            if ( Params::VisibleBoundaryMesh )
-              {
-                auto* renderer = new kvs::PolygonRenderer();
-                renderer->setTwoSideLightingEnabled( true );
-                screen.registerObject( mesh, renderer );
-              }
        }
    };
}
```
として下さい．

編集が終わったら
```
$ wclean && wmake
```
でビルドします．

### 解析の実行
解析ディレクトリに移動します．
```
$ cd ~/local/calc/vis_propeller
```
解析の実行です．
```
$ mpirun -np 4 propeller_pimpleFoam -parallel
```
環境にも依りますが，計算に非常に長い時間がかかりますので，
```
$ nohup mpirun -np 4 propeller_pimpleFoam -parallel 2>&1 log.dat &
```
として
```
$ tail -f log.dat
```
で監視しても良いです．

### 結果例
<img width="512" height="512" alt="propeller_iso" src="https://github.com/user-attachments/assets/e39b5150-08e7-430c-a5ed-91b8c3478fa3" />
