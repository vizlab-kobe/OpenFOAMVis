# OpenFOAMのセットアップ

## 準備
- Ubuntu 18.04 LTS, 22.04 LTS, 24.04LTSで動作確認済みです．
- ディレクトリ階層を以下の通りとします．
```
└── $HOME/
    ├── local
    └── Work/
        └── GitHub
```

## OpenFOAM v2412 の環境構築
以下のライブラリを後述の通り準備します．
- gcc
- cmake
- openmpi
- zlib
- flex
- m4
- bison

その後，OpenFOAM ThirdParty，およびOpenFOAM本体のビルドをします．

### gccの確認
- gcc-7.5.0, gcc-10.5.0，およびgcc-11.4.0にて動作確認済みです．
- Ubuntu 22.04 LTSにはgcc-11.4.0がプリセットされています．そのまま使用することができます．
- Ubuntu 24.04 LTSにプリセットされているgcc 13では動作確認をしていません．11.4.0を導入して下さい．
``` 
$ gcc --version
gcc (Ubuntu 11.4.0-1ubuntu1~22.04.3) ll.4.0
...
```

### cmakeのビルド
llvm，およびOpenFOAMをビルドするのに必要です．3.27.9で動作確認済みです．
``` 
$ cd ~/Work/Github
$ wget https://github.com/Kitware/CMake/releases/download/v3.27.9/cmake-3.27.9.tar.gz
$ tar –xvf cmake-3.27.9.tar.gz
$ cd cmake-3.27.9
$ mkdir build
$ cd build
$ ../bootstrap -–prefix=$HOME/local/cmake-3.27.9
$ make –j
$ make install
```

make installまで終了後，`~/.bashrc`にパスを記述する必要があります．
以下の内容を`~/.bashrc`の末尾に記述しておきます．
```bash
export PATH=$HOME/local/cmake-3.27.9/bin:$PATH
```

terminalに戻って`~/.bashrc`を読み込みます．
``` 
$ source ~/.bashrc
```

### openmpiのビルド
並列計算に必要です．OpenFOAMのビルド前にインストールしておく必要があります．
4.1.2で動作確認済みです．terminalで以下のコマンドを入力します．
``` 
$ cd ~/Work/Github
$ wget https://download.open-mpi.org/release/open-mpi/v4.1/openmpi-4.1.2.tar.gz
$ tar -xvf openmpi-4.1.2.tar.gz 
$ cd openmpi-4.1.2
$ mkdir build
$ cd build
$ ../configure CC=gcc CXX=g++ F77=gfortran FC=gfortran --prefix=$HOME/local/openmpi-4.1.2
$ make –j
$ make install
```

成功後，`~/.bashrc`に以下を追加します．コンパイラ (gcc/g++)がzlibを見つけられるようにします．
```bash
export ZLIB_DIR=$HOME/local/zlib-1.3.1
export CPATH=$ZLIB_DIR/include:$CPATH
export LIBRARY_PATH=$ZLIB_DIR/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=$ZLIB_DIR/lib:$LD_LIBRARY_PATH
```

terminalに戻って`~/.bashrc`を読み込みます．
```bash
source ~/.bashrc
```

### zlibのビルド
zlibをビルドします．zlibはOpenFOAM形式の圧縮ファイルの処理に必要です．
``` 
$ cd ~/Work/Github/
$ wget https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz
$ tar –xvf zlib-1.3.1.tar.gz
$ cd zlib-1.3.1
$ mkdir build
$ cd build
$ ../configure --prefix=$HOME/local/zlib-1.3.1
$ make –j
$ make install
```

成功後，`~/.bashrc`に以下を追加します．コンパイラ (gcc/g++)がzlibを見つけられるようにします．
```bash
export ZLIB_DIR=$HOME/local/zlib-1.3.1
export CPATH=$ZLIB_DIR/include:$CPATH
export LIBRARY_PATH=$ZLIB_DIR/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=$ZLIB_DIR/lib:$LD_LIBRARY_PATH
```

terminalに戻って`~/.bashrc`を読み込みます．
``` 
source ~/.bashrc
```

### flexのビルド
flexをビルドします．flexはOpenFOAMの設定ファイルを読み込むためのライブラリです．
``` 
$ cd ~/Work/Github 
$ wget https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz 
$ tar -xvf flex-2.6.4.tar.gz 
$ cd flex-2.6.4 
$ mkdir build
$ cd build
$ ../configure --prefix=$HOME/local/flex-2.6.4
$ make –j
$ make install
```

※ glibcが2.26~2.28あたりの環境では`configure`コマンド実行時にエラーが発生するようです．上記の代わりに
```
$ ../configure --prefix=$HOME/local/flex-2.6.4 CFLAGS="-D_GNU_SOURCE" ac_cv_func_reallocarray=yes
```
としてください．glibcのバージョンは```ldd --version```で確認可能です．

成功後，`~/.bashrc`に以下を追加します．コンパイラ (gcc/g++)がflexを見つけられるようにします．
```bash
export PATH=$HOME/local/flex-2.6.4/bin:$PATH
export CPATH=$HOME/local/flex-2.6.4/include:$CPATH
```

terminalに戻って`~/.bashrc`を読み込みます．
``` 
source ~/.bashrc
```

### m4のビルド
OpenFOAM ThirdPartyであるgmpのビルドに必要です．1.4.18, 19で動作確認済みです．Ubuntu 22.04 LTS等の新しい環境では1.4.18以下のバージョンはコンパイル時にエラーが発生するようです．その場合には1.4.19以上のバージョンを使って下さい．terminalで以下のコマンドを入力します．
``` 
$ cd ~/Work/Github
$ wget https://ftp.gnu.org/gnu/m4/m4-1.4.19.tar.gz
$ tar –xvf m4-1.4.19.tar.gz
$ cd m4-1.4.19
$ mkdir build
$ cd build
$ ../configure –-prefix=$HOME/local/m4-1.4.19
$ make –j
$ make install
```

成功後，`~/.bashrc`に以下を追加します．
```bash
export PATH=$HOME/local/m4-1.4.19/bin:$PATH 
```

terminalに戻って`~/.bashrc`を読み込みます．
``` 
source ~/.bashrc
```

### bisonのビルド
bisonをビルドします．bisonは数式をC++ (OpenFOAM) 向けに読み取るライブラリです．
``` 
$ cd ~/Work/Github 
$ wget https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.gz 
$ tar -xvf bison-3.8.2.tar.gz
$ cd bison-3.8.2 
$ mkdir build
$ cd build
$ ../configure –-prefix=$HOME/local/bison-3.8.2 
$ make -j 
$ make install
```

成功後，`~/.bashrc`に以下を追加します．コンパイラ (gcc/g++)がflexを見つけられるようにします．
```bash
export PATH=$HOME/local/bison-3.8.2/bin:$PATH 
```

terminalに戻って`~/.bashrc`を読み込みます．
```bash
source ~/.bashrc
```

## OpenFOAMの準備
- ESI版のv2412を使用します．
https://www.openfoam.com/download/release-history
からダウンロードできます．

``` 
$ cd ~/Work/Github
$ wget https://dl.openfoam.com/source/v2412/OpenFOAM-v2412.tgz
$ tar –xvf OpenFOAM-v2412.tgz
$ wget https://dl.openfoam.com/source/v2412/ThirdParty-v2412.tgz
$ tar –xvf ThirdParty-v2412.tgz
```

(参考：https://qiita.com/yotakagi77/items/17006fd0dedef3acc573)

### OpenFOAM ThirdPartyのビルド
まずOpenFOAM-v2412のビルドに必要なライブラリをThirdPartyを用いてビルドします．
``` 
$ cd ~/Work/Github/ThirdParty-v2412
```

gmp，mpfr, mpc, cmake, qtをダウンロードします．
``` 
$ wget https://ftp.gnu.org/gnu/gmp/gmp-6.2.0.tar.xz
$ tar –xvf gmp-6.2.0.tar.xz
$ wget https://ftp.gnu.org/gnu/mpfr/mpfr-4.0.2.tar.xz
$ tar –xvf mpfr-4.0.2.tar.xz
$ wget https://ftp.gnu.org/gnu/mpc/mpc-1.1.0.tar.gz
$ tar –xvf mpc-1.1.0.tar.gz 
$ wget https://download.qt.io/archive/qt/5.12/5.12.11/single/qt-everywhere-src-5.12.11.tar.xz
$ tar -xvf qt-everywhere-src-5.12.11.tar.xz
$ mv qt-everywhere-src-5.12.11 qt-everywhere-opensource-src-5.12.11 
```

`../OpenFOAM-v2412/etc/config.sh/compiler`を以下のように書き換えます．
```bash
61: default_clang_version=llvm-15.0.7
62: default_gcc_version=gcc-10.5.0
...
64: default_gmp_version=gmp-6.2.0
65: default_mpfr_version=mpfr-4.0.2
66: default_mpc_version=mpc-1.1.0
```

OpenFOAMの`bashrc`を読み込みます．
``` 
$. ~/Work/Github/OpenFOAM-v2412/etc/bashrc
```
### 必要なライブラリのビルド
CGALのビルドを行います．CGALはsnappyhexMeshなど，解析をするためのメッシュ切りの際に必要です．
可視化にはそれほど重要では有りませんので，必要がなければ飛ばしてOKです．
まずgmpのビルドを行います．
``` 
$ cd ~/Work/Github/ThirdParty-v2412/gmp-6.2.0
$ mkdir build
$ cd build
$ ../configure --prefix=$WM_THIRD_PARTY_DIR/platforms/$WM_ARCH/gmp-6.2.0 --enable-cxx --with-pic 
$ make -j
$ make install
```

次にmpfrのビルドを行います．先ほどビルドしたgmpを使用します．
``` 
$ cd ~/Work/Github/ThirdParty-v2412/mpfr-4.0.2
$ mkdir build
$ cd build
$ ../configure --prefix=$WM_THIRD_PARTY_DIR/platforms/$WM_ARCH/mpfr-4.0.2 --with-gmp=$WM_THIRD_PARTY_DIR/platforms/$WM_ARCH/gmp-6.2.0 --with-pic 
$ make -j
$ make install
```

最後にCGALのビルドを行います．gmpとmpfrを用いてビルドをします．
``` 
$ cd ~/Work/Github/ThirdParty-v2412
$ ./makeCGAL CGAL-4.14.3 gmp-6.2.0 mpfr-4.0.2
```

### Third Partyのビルド
`../OpenFOAM-v2412/etc/bashrc`の以下の箇所を書き換えます．
```bash
106: export WM_MPLIB=SYSTEMOPENMPI
...
182: export WM_PROJECT_USER_DIR="$HOME/local/$WM_PROJECT/${USER:-user}-$WM_PROJECT_VERSION"
```

OpenFOAMの`bashrc`を読み込みます．
``` 
$ cd ~/Work/Github/ThirdParty-v2412
$. ~/Work/Github/OpenFOAM-v2412/etc/bashrc
```
その後ビルドをします．
``` 
$ ./Allwmake
```

Addios関係でエラーが起きた場合は
``` 
$ ./makeAdios2 -cmake $HOME/local/cmake-3.27.9/bin
```
を実行したあと，再度
``` 
$ ./Allwmake
```
を行って下さい．なお，Adiosは大規模並列計算向けのライブラリです．不要なら無視して下さい．

ThirdPartyが問題なくビルドされたことを確認します．
``` 
$ foamSystemCheck
```
を実行し，
``` 
System check: PASS
==================
Can continue to OpenFOAM installation
```
と表示さればOKです．OpenFOAM本体のビルドに進みます．

## OpenFOAM本体のビルド
再度OpenFOAMのbashrcを読み込みます．
``` 
$ cd ~/Work/Github/OpenFOAM-v2412
$ . ./etc/bashrc
```
ビルドを開始します．
``` 
$ ./Allwmake -j
```
長い時間が経過し，以下のようなメッセージを得るとビルドが完了です．
``` 
==============================================
 OpenFOAM-v2412
 Gcc system compiler
 linux64GccDPInt640pt, with SYSTEMOPENMPI sys-openmpi

 api = 2412
 patch = 0
 bin = 287 entries
 lib = 154 entries

==============================================
```

最後に`~/.bashrc`に以下を追加します
```bash
. $HOME/Work/Github/OpenFOAM-v2412/etc/bashrc
```
terminalに戻って~/.bashrcを読み込みます．
``` 
$ source ~/.bashrc
```

# KVSのビルド
## KVSの入手
KVSを入手します．
``` 
$ cd ~/Work/GitHub
$ git clone https://github.com/naohisAas/KVS.git
```
`~/.bashrc`に以下を追加します．
```bash
export KVS_DIR=$HOME/local/kvs
export PATH=$KVS_DIR/bin:$PATH
unset DISPLAY
```
terminalに戻って~/.bashrcを読み込みます．
``` 
$ source ~/.bashrc
```


## ビルドについての検討
2つ作戦があります．
1. EGLの使用

   GPUが使用できる環境ならばEGLが使用可能です．
   
2. OSMesaの使用

   GPUが使用できない場合はOSMesaを使用することになります．

これらは**二者択一**です．


## EGL版のKVSのビルド
KVSディレクトリに移動します．
``` 
cd ~/Work/GitHub/KVS
```
OpenFOAMのIn-Situ可視化用途でEGL版KVSを使用する場合には，`kvs.conf`を以下のように設定します：
```bash
KVS_ENABLE_OPENGL     = 1
KVS_ENABLE_GLU        = 0
KVS_ENABLE_GLEW       = 0
KVS_ENABLE_OPENMP     = 1
KVS_ENABLE_DEPRECATED = 0

KVS_SUPPORT_CUDA      = 0
KVS_SUPPORT_GLUT      = 0
KVS_SUPPORT_GLFW      = 0
KVS_SUPPORT_FFMPEG    = 0
KVS_SUPPORT_OPENCV    = 0
KVS_SUPPORT_QT        = 0
KVS_SUPPORT_PYTHON    = 0
KVS_SUPPORT_MPI       = 1
KVS_SUPPORT_EGL       = 1
KVS_SUPPORT_OSMESA    = 0
```
terminalにて以下のコマンドを実行します（管理者権限が必要です）：
``` 
$ sudo apt install libegl1-mesa-dev
```
`/etc/glvnd/egl_vendor.d/10_nvidia.json`に以下を記述します．ファイルがなければ作成します．こちらも管理者権限が必要です．
```bash
{
    "file_format_version" : "1.0.0",
    "ICD":{
        "library_path": "libEGL_nvidia.so.0"
    }
}
```
その後ビルドをします．
``` 
$ make clean
$ make
$ make install
```
エラーが出なければビルド成功です．

## OSMesa版のKVSのビルド

### ninjaの準備
llvmをビルドするのに必要です．terminalで以下の通り入力します．最終的にcmakeのライブラリに格納します．
``` 
$ cd ~/Work/Github
$ git clone https://github.com/ninja-build/ninja.git
$ cd ninja
$ python3 configure.py –bootstrap
$ cp ninja $HOME/local/cmake-3.27.9/bin
```

### llvmのビルド
OSMesaを使用する際に，llvmpipeを使って高速可視化を実現します．15.0.7で動作確認済みです．
terminalで以下のコマンドを入力します．
``` 
$ cd ~/Work/Github
$ git clone https://github.com/llvm/llvm-project.git
$ cd llvm-project
$ git checkout llvmorg-15.0.7
```
さらに
``` 
$ cmake -G Ninja -DCMAKE_INSTALL_PREFIX=$HOME/local/llvm-15.0.7 \  
-DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_RTTI=ON \  
-DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_LINK_LLVM_DYLIB=ON \  
-DLLVM_TARGETS_TO_BUILD="X86" \
../llvm
```
を実行します．なお`-DLLVM_TARGETS_TO_BUILD="X86"`の部分については，各自の環境に合わせた設定が必要です．terminalで
``` 
uname -m
```
のコマンドで確認して下さい．

cmakeが無事に終了した後
``` 
$ ninja -j
$ ninja install
```
正常に終了後，`~/.bashrc`に以下の内容を追加しておきます．
```bash
export LLVM_PATH=$HOME/local/llvm-15.0.7
export PATH=$LLVM_PATH/bin:$PATH
export LD_LIBRARY_PATH=$LLVM_PATH/lib:$LD_LIBRARY_PATH
```
terminalに戻り`~/.bashrc`を読み込みます．
``` 
$ source ~/.bashrc
```

### OSMesaのビルド
OSMesa 22.3.7で動作確認済みです．これをビルドするためにはmeson, ninja, makoが必要です．
``` 
$ pip3 install –user meson ninja mako
```
これらのライブラリが`$HOME/.local`に入るので，`~/.bashrc`にパスを通します．
```bash
export PATH=$HOME/.local/bin:$PATH
```
再びterminalでOSMesaを取得します．
``` 
$ cd ~/Work/Github
$ wget https://archive.mesa3d.org/older-versions/22.x/mesa-22.3.7.tar.xz
$ tar –xvf mesa-22.3.7.tar.xz
$ cd mesa-22.3.7
```
以下のコマンドでビルドします．
``` 
$ meson setup build \
  --prefix=$HOME/local/osmesa_22.3.7 \
  --buildtype=release \
  -Dosmesa=true \
  -Dgallium-drivers=swrast \
  -Ddri-drivers= \
  -Dvulkan-drivers= \
  -Dplatforms= \
  -Dglx=disabled \
  -Degl=disabled \
  -Dgbm=disabled \
  -Dshared-glapi=enabled \
  -Dllvm=enabled
$ ninja –C build
$ ninja –C build install
```
KVSでOSMesaを使用するため，以下を`~/.bashrc`に追加します．
```bash
export LLVM_CONFIG=$LLVM_PATH/bin/llvm-config
export KVS_OSMESA_DIR=$HOME/local/osmesa_22.3.7
export KVS_OSMESA_LINK_LIBRARY=“-lOSMesa –lz $($LLVM_CONFIG --ldflags) \
$($LLVM_CONFIG --libs all) $($LLVM_CONFIG --system-libs) –lrt –ldl –lpthread –lm”
export LD_LIBRARY_PATH=$HOME/local/osmesa_22.3.7/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
```
### OSMesa版のKVSのビルド
KVSディレクトリに移動します．
``` 
cd ~/Work/GitHub/KVS
```
`kvs.conf`を以下のように編集します：
```bash
KVS_ENABLE_OPENGL     = 1
KVS_ENABLE_GLU        = 0
KVS_ENABLE_GLEW       = 0
KVS_ENABLE_OPENMP     = 1
KVS_ENABLE_DEPRECATED = 0

KVS_SUPPORT_CUDA      = 0
KVS_SUPPORT_GLUT      = 0
KVS_SUPPORT_GLFW      = 0
KVS_SUPPORT_FFMPEG    = 0
KVS_SUPPORT_OPENCV    = 0
KVS_SUPPORT_QT        = 0
KVS_SUPPORT_PYTHON    = 0
KVS_SUPPORT_MPI       = 1
KVS_SUPPORT_EGL       = 0
KVS_SUPPORT_OSMESA    = 1
```
その後以下のようにビルドします．
``` 
$ make clean
$ make
$ make install
```
エラーが出なければビルド成功です．

# InSituVisの準備
InSituVisのライブラリを準備してビルドします．
``` 
$ cd ~/Work/Github
$ git clone https://github.com/vizlab-kobe/InSituVis.git
$ cd InSituVis/Lib
$ python3 kvsmake.py
```
`~/Work/Github/InSituVis/LibにlibInSituVis.a`が生成されていればOKです．

# OralAirFlowVis可視化
OralAirFlowVisを入手します
``` 
$ cd ~/Work/Github
$ git clone https://github.com/vizlab-kobe/OralAirFlowVis.git
$ cd OralAirFlowVis
```
今回はOralAirFlowVisに含まれる解析realistic-s3について，圧力pの等値面を可視化します．

## ソルバーの改造
OralAirFlowVisに含まれているrhoPimpleFoamはOpenFOAM 2.3.1に対応したものであり，そのままOpenFOAM v2412で使用することはできません．
OpenFOAM v2412のソルバーを改造します．

``` 
$ cp –r $FOAM_SOLVERS/compressible/rhoPimpleFoam ./my_rhoPimpleFoam
$ cd my_rhoPimpleFoam
$ cp ../rhoPimpleFoam_InSituVis/InSituVis.h .
```

`rhoPimpleFoam.C`を`OralAirFlowVis/rhoPimpleFoam_InSituVis/rhoPimpleFoam.C`を参考に改造します．

追記は計5箇所です．

1つ目．ヘッダー部分は
```cpp
#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "fluidThermo.H"
#include "turbulentFluidThermoModel.H"
#include "bound.H"
#include "pimpleControl.H"
#include "pressureControl.H"
#include "CorrectPhi.H"
#include "fvOptions.H"
#include "localEulerDdtScheme.H"
#include "fvcSmooth.H"

// In-situ visualization
#define IN_SITU_VIS
#if defined( IN_SITU_VIS )
#include "InSituVis.h"
#include <InSituVis/Lib.foam/FoamToKVS.h>

// IN_SITU_VIS__P: Pressure
// IN_SITU_VIS__U: Velocity
// IN_SITU_VIS__T: Temperature
#define IN_SITU_VIS__P
#endif
```
とします．**`#define IN_SITU_VIS`から`#endif`までが追記内容**です．以下同じです．今回は圧力Pの可視化を試みます．

2つ目．main関数内部にて
```cpp
    turbulence->validate();

    if (!LTS)
    {
        #include "compressibleCourantNo.H"
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
```
を追記します．

3つ目．時間発展のwhile文の手前です．
```cpp
        if (LTS)
        {
            #include "setRDeltaT.H"
        }
        else
        {
            #include "compressibleCourantNo.H"
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
```

4つ目．時間発展のwhile文の内部です．最も重要な部分です．各格子ごとにvis.putで可視化パイプラインに引き渡し，具体的な可視化を行います．
```cpp
        rho = thermo.rho();

        runTime.write();

#if defined( IN_SITU_VIS )
        vis.simTimer().stamp();
        const auto ts = vis.simTimer().last();
        const auto Ts = kvs::String::From( ts, 4 );
        vis.log() << indent << "Processing Times:" << std::endl;
        vis.log() << indent.nextIndent() << "Simulation: " << Ts << " s" << std::endl;

        // Execute in-situ visualization process
#if defined( IN_SITU_VIS__P ) // p: pressure
        auto& field = p;
        // A (whole min/max values)
        //const auto min_value = 97928.796875;
        //const auto max_value = 106227.53906;
        //const auto min_value = 0.99998 * 100000.0;
        //const auto max_value = 1.02000 * 100000.0;
        // B
        //const auto min_value = 9.94 * 10000.0;
        //const auto max_value = 1.02 * 100000.0;
        // C
        //const auto min_value = 0.999999 * 100000.0;
        //const auto max_value = 1.000020 * 100000.0;
        const auto min_value = 0.999990 * 100000.0;
        const auto max_value = 1.000200 * 100000.0;
#elif defined( IN_SITU_VIS__U ) // U: velocity
        auto& field = U;
        // A (whole min/max values)
        const auto min_value = 0.0;
        const auto max_value = 71.645393372;
        //const auto min_value = 0.0224;
        //const auto max_value = 70.9;
#elif defined( IN_SITU_VIS__T ) // T: temperature
        auto& field = thermo.T();
        // A (whole min/max values)
        const auto min_value = 289.91583252;
        const auto max_value = 296.15917969;
        //const auto min_value = 290.0;
        //const auto max_value = 296.16;
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

5つ目．解析終了後に出力するログです．
```cpp
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
```
## ソルバーのコンパイルの設定
### `Make/files`の編集
`Make/files`を以下の通り編集します
```bash
rhoPimpleFoam.C

EXE = rhoPimpleFoam
```

なお，ここで改造したソルバーをOralAirFlow以外の別の解析で使用したい場合には
```bash
rhoPimpleFoam.C

EXE = $(FOAM_USER_APPBIN)/my_PimpleFoam
```
とします．上記の設定をすれば，`my_PimpleFoam`というコマンドがローカル環境に登録されます．

### `Make/options`の編集（EGL版）
EGL版のKVSを使用する場合には以下の通り編集して下さい．
```diff
EXE_INC = \
（数行省略）
    -I$(LIB_SRC)/TurbulenceModels/compressible/lnInclude \
    -I$(LIB_SRC)/regionFaModels/lnInclude \
+    -I$(HOME)/local/openmpi-4.1.2/include \ 

EXE_LIBS = \
（省略）


/* KVS settings (EGL / GPU Mode) */
+EXE_INC += \
+    -I${KVS_DIR}/include -DKVS_SUPPORT_MPI -DKVS_USE_MPI \
+    -DKVS_SUPPORT_EGL -DEGL_NO_X11 -DMESA_EGL_NO_X11_HEADERS

+EXE_LIBS += \
+    -L${KVS_DIR}/lib -lkvsSupportMPI \
+    -lkvsSupportEGL -lkvsCore \
+    -lEGL -lGL

/* InSitu settings */
+EXE_INC += -I$(HOME)/Work/Github
+EXE_LIBS += -L$(HOME)/Work/Github/InSituVis/Lib -lInSituVis

/* OpenMP settings */
+EXE_INC += -fopenmp
+EXE_LIBS += -fopenmp
```
### `Make/options`の編集（OSMesa版）
OSMesa版のkVSを使用する場合には以下のとおり編集して下さい

```diff
EXE_INC = \
（数行省略）
    -I$(LIB_SRC)/regionFaModels/lnInclude \
+    -I$(HOME)/local/openmpi-4.1.2/include \

EXE_LIBS = \
（省略）

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

### ビルドの実行
terminalで
``` 
$ wclean && wmake
```
でビルドします．エラーが出なければ成功です．

## OralAirFlowVisの可視化
可視化をします．ここでは例としてOralAirFlowVisに含まれるrealistic-s3を使用します．
``` 
$ cd ~/Work/GitHub/OralAirFlowVis/realistic-s3
```

### 解析ファイルの編集
OralAirFlowVisはOpenFOAM 2.3.1向けの記述が残っているため，そのままでは動きません．OpenFOAM v2412で動作するように解析ファイルにも手を入れる必要があります．
constant/turbulencePropertiesを以下の通り改造します．乱流モデルの設定です．

まずconstant/turbulencePropertiesを以下のとおり改造します．乱流モデルの設定です．
```diff
- simulationType LESModel;
+ simulationType LES;
+ {
+   LESModel WALE;
+   turbulence on;
+   printCoeffs on;
+   delta cubeRootVol;
+}
```
次に初期条件の設定ファイル名を変更します．
``` 
$ rm -r processor*
$ mv 0/alphaSgs 0/alphat
$ mv 0/muSgs 0/nut
```
`./0/nut`を編集します．

```diff
FoamFile
{
    version     2.0;
    format      ascii;
    class       volScalarField;
-    object      muSgs;
+    location    "0";
+    object      nut;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
-dimensions      [1 -1 -1 0 0 0 0];
+dimensions      [0 2 -1 0 0 0 0];

internalField   uniform 0;

```

`./system/fvSchemes`を以下のとおり編集します．
```diff
divSchemes
{
    default         none;
    div(phi,U)      Gauss LUST grad(U);
    div(phi,e)      Gauss LUST grad(e);
    div(phi,K)      Gauss linear;
    div(phiv,p)     Gauss linear;
    div(phi,k)      Gauss limitedLinear 1;
    div(phi,B)      Gauss limitedLinear 1;
    div(phi,muTilda) Gauss limitedLinear 1;
    div(B)          Gauss linear;
    div((muEff*dev2(T(grad(U))))) Gauss linear;
    div(div(((rho*U)*U)))  Gauss linear;
    div(((rho*U)*U))   Gauss linear;
+    div(((rho*nuEff)*dev2(T(grad(U))))) Gauss linear;
}
```
### 実行
並列計算向けにメッシュ分割を行います
``` 
$ decomposePar
```

`run_insitu.sh`を以下の内容に編集します．
```bash
#!/bin/sh

#unset FOAM_SIGFPE
export FOAM_SIGFPE=false
mpirun -n 8 ../my_rhoPimpleFoam/rhoPimpleFoam -parallel
```

実行します．
``` 
$ ./run_insitu.sh
```
`Output`に画像が出力されます．

### 実行結果の例

<img width="512" height="512" alt="OralAirFlowVis" src="https://github.com/user-attachments/assets/b6fc3d9b-23ce-46cc-9ca9-892fb14936d5" />


# OpenFOAM tutorial解析の可視化（5/26以降執筆予定）
## motorBikeの解析

## propellerの解析
