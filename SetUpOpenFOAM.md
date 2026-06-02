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

### gccの確認（検証中）
- gcc-7.5.0, gcc-10.5.0，およびgcc-11.4.0にて動作確認済みです．
- Ubuntu 22.04 LTSにはgcc-11.4.0がプリセットされています．そのまま使用することができます．
- Ubuntu 24.04 LTSにプリセットされているgcc 13では動作確認をしていません．
- 自前環境に独自ビルドしたgccを使用すると，OSMesaを準備する際のllvmのビルドの段階で，システムのgccと競合することがあるようです．できるだけシステムに予めビルドされているgccを使用して下さい（検証中）．
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

成功後，`~/.bashrc`に以下を追加します．コンパイラ (gcc/g++)がopenmpiを見つけられるようにします．
```bash
export MPI_PATH=$HOME/local/openmpi-4.1.2
export PATH=$MPI_PATH/bin:$PATH
export LD_LIBRARY_PATH=$MPI_PATH/lib:$LD_LIBRARY_PATH
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

きちんとビルドされたことを確認します．例えば
```
$ which pimpleFoam
```
に対して
```
~/Work/GitHub/OpenFOAM-v2412/platforms/linux64GccDPInt320pt/bin/pimpleFoam
```
と出ていればOKです．
