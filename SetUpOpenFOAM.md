<img width="437" height="39" alt="image" src="https://github.com/user-attachments/assets/f090acc2-9fe0-48fe-80c0-3a3cd5de5691" /><img width="714" height="42" alt="image" src="https://github.com/user-attachments/assets/c019f5c2-f3db-4310-aff4-602be38b9a5e" /># OpenFOAMのセットアップ

## 準備
- Ubuntu 18.04 LTS, 22.04 LTS, 24.04LTSで動作確認済みです．
- ディレクトリ階層を以下の通りとします．
```
~/local/Work/GitHub
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
- gcc-10.5.0，およびgcc-11.4.0にて動作確認済みです．
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

make installまで終了後，~/.bashrcにパスを記述する必要があります．
以下の内容を~/.bashrcの末尾に記述しておきます．
```
export PATH=$HOME/local/cmake-3.27.9/bin:$PATH
```

terminalに戻って~/.bashrcを読み込みます．
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

成功後，~/.bashrcに以下を追加します．コンパイラ (gcc/g++)がzlibを見つけられるようにします．
```
export ZLIB_DIR=$HOME/local/zlib-1.3.1
export CPATH=$ZLIB_DIR/include:$CPATH
export LIBRARY_PATH=$ZLIB_DIR/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=$ZLIB_DIR/lib:$LD_LIBRARY_PATH
```

terminalに戻って~/.bashrcを読み込みます．
```
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

成功後，~/.bashrcに以下を追加します．コンパイラ (gcc/g++)がzlibを見つけられるようにします．
```
export ZLIB_DIR=$HOME/local/zlib-1.3.1
export CPATH=$ZLIB_DIR/include:$CPATH
export LIBRARY_PATH=$ZLIB_DIR/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=$ZLIB_DIR/lib:$LD_LIBRARY_PATH
```

terminalに戻って~/.bashrcを読み込みます．
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

成功後，~/.bashrcに以下を追加します．コンパイラ (gcc/g++)がflexを見つけられるようにします．
```
export PATH=$HOME/local/flex-2.6.4/bin:$PATH
export CPATH=$HOME/local/flex-2.6.4/include:$CPATH
```

terminalに戻って~/.bashrcを読み込みます．
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

成功後，~/.bashrcに以下を追加します．
```
export PATH=$HOME/local/m4-1.4.19/bin:$PATH 
```

terminalに戻って~/.bashrcを読み込みます．
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

成功後，~/.bashrcに以下を追加します．コンパイラ (gcc/g++)がflexを見つけられるようにします．
```
export PATH=$HOME/local/bison-3.8.2/bin:$PATH 
```

terminalに戻って~/.bashrcを読み込みます．
```
source ~/.bashrc
```

## OpenFOAMの準備
- ESI版のv2412を使用します．
https://www.openfoam.com/download/release-history
からダウンロードできます．

```
$ cd ~/Work/Github
$ wget https://dl.openfoam.com/source/v2412/OpenFOAM-v2412.tgz
$ tar –xvf OpenFOAM-v2412
$ wget https://dl.openfoam.com/source/v2412/ThirdParty-v2412.tgz
$ tar –xvf ThirdParty-v2412
```

(参考：https://qiita.com/yotakagi77/items/17006fd0dedef3acc573)

### OpenFOAM ThirdPartyのビルド
まずOpenFOAM-v2412のビルドに必要なライブラリをThirdPartyを用いてビルドします．
```
$ cd ~/Work/Github/ThirdParty-v2412
```

https://qiita.com/yotakagi77/items/17006fd0dedef3acc573の情報に従って，gmp，mpfr, mpc, cmake, qtをダウンロードします．
```
$ wget https://ftp.gnu.org/gnu/gmp/gmp-6.2.0.tar.xz
$ tar –xvf gmp-6.2.0.tar.gz
$ wget https://ftp.gnu.org/gnu/mpfr/mpfr-4.0.2.tar.xz
$ tar –xvf mpfr-4.0.2.tar.xz
$ wget https://ftp.gnu.org/gnu/mpc/mpc-1.1.0.tar.gz
$ tar –xvf mpc-1.1.0.tar.gz 
$ wget https://download.qt.io/archive/qt/5.12/5.12.11/single/qt-everywhere-src-5.12.11.tar.xz
$ tar -xvf qt-everywhere-src-5.12.11.tar.xz
$ mv qt-everywhere-src-5.12.11 qt-everywhere-opensource-src-5.12.11 
```

../OpenFOAM-v2412/etc/config.sh/compilerを以下のように書き換えます．
```
61: default_clang_version=llvm-15.0.7
62: default_gcc_version=gcc-10.5.0
...
64: default_gmp_version=gmp-6.2.0
65: default_mpfr_version=mpfr-4.0.2
66: default_mpc_version=mpc-1.1.0
```

OpenFOAMのbashrcを読み込みます．
```
$. ~/Work/Github/OpenFOAM-v2412/etc/bashrc
```
