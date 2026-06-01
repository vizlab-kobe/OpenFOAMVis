# KVSのビルド
基本的には[KVS](https://github.com/naohisas/KVS)を参照してください．

## ビルドするライブラリについて
本ドキュメントはOpenFOAM v2412でin-situ可視化をするための準備として記述しています．以下の順番で各ライブラリをビルドします．依存関係は....

coming soon...


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
$ python3 configure.py –-bootstrap
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
$ mkdir build
$ cd build
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

## 動作確認


# InSituVisの準備
InSituVisのライブラリを準備してビルドします．
``` 
$ cd ~/Work/Github
$ git clone https://github.com/vizlab-kobe/InSituVis.git
$ cd InSituVis/Lib
$ python3 kvsmake.py
```
`~/Work/Github/InSituVis/LibにlibInSituVis.a`が生成されていればOKです．

# 動作確認

## OpenFOAMv2412を用いたOralAirFlowVisの可視化
coming soon..

## OpenFOAMのtutorialの可視化
coming soon..
