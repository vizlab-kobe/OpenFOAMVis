# KVSのビルド
基本的には[KVS](https://github.com/naohisas/KVS)を参照してください．本ドキュメントはOpenFOAM v2412でin-situ可視化をするための準備として記述しています．

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
terminalに戻って`~/.bashrc`を読み込みます．
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
### 動作確認
以下のコマンドで動作確認が可能です．
```
$ cd ~/Work/GitHub/KVS/Example/SupportEGL/Hello
$ kvsmake -G
$ kvsmake
```
これにより`output_0??.bmp`が11枚出力されます．InSituVisの準備の項へ進んで下さい．

## OSMesa版のKVSのビルド
ここでは
- ninja
- llvm
- osmesa
を用意します．

これらは依存関係があります．必ず`ninja`→`llvm`→`osmesa`の順番で実施して下さい．

### ninjaの取得
まずは`ninja`を取得します．あとで必要になるので，ついでに`meson`, `mako`も取得します．次のコマンドで取得可能です：
``` 
$ pip3 install --user meson ninja mako --break-system-packages
```
Ubuntu18.04など，すこし古いLinuxの場合には
```
$ pip3 install --user meson ninja mako
```
でOKです．

これらのライブラリが`$HOME/.local`に入るので，`~/.bashrc`にパスを通します．
```bash
export PATH=$HOME/.local/bin:$PATH
```
terminalに戻り反映します．
```
$ source ~/.bashrc
```

### llvmのビルド
`OSMesa`を使用する際に，`llvmpipe`を使って高速可視化を実現します．15.0.7で動作確認済みです．
terminalで以下のコマンドを入力します．
``` 
$ cd ~/Work/GitHub
$ git clone https://github.com/llvm/llvm-project.git
$ cd llvm-project
$ git checkout llvmorg-15.0.7
```
さらに
```
$ mkdir build
$ cd build
$ CC=gcc CXX=g++ cmake -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_INSTALL_PREFIX=$HOME/local/llvm-15.0.7 -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_RTTI=ON -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_LINK_LLVM_DYLIB=ON -DLLVM_TARGETS_TO_BUILD="X86" ../llvm
```
を実行します．先ほどビルドした`ninja`を使用しています．

cmakeが無事に終了した後
``` 
$ ninja
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
確認として，
```
$ llvm-config --version
```
と入力した際に`15.0.7`と出ればOKです．

### OSMesaのビルド
OSMesa 22.3.7で動作確認済みです．

terminalでOSMesaを取得します．
``` 
$ cd ~/Work/GitHub
$ wget https://archive.mesa3d.org/older-versions/22.x/mesa-22.3.7.tar.xz
$ tar -xvf mesa-22.3.7.tar.xz
$ cd mesa-22.3.7
```
以下のコマンドでビルドします．
``` 
$ CC=gcc CXX=g++ meson setup build \
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
$ ninja -C build
$ ninja -C build install
```
KVSでOSMesaを使用するため，以下を`~/.bashrc`に追加します．
```bash
export LLVM_CONFIG=$LLVM_PATH/bin/llvm-config
export KVS_OSMESA_DIR=$HOME/local/osmesa_22.3.7
export LD_LIBRARY_PATH=$KVS_OSMESA_DIR/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
export KVS_OSMESA_LINK_LIBRARY="-lOSMesa -lglapi -lz $($LLVM_CONFIG --ldflags) $($LLVM_CONFIG --libs all) $($LLVM_CONFIG --system-libs) -lrt -ldl -lpthread -lm"
```
ターミナルに戻り環境変数を反映します．
```
$ source ~/.bashrc
```

確認
```
$ ls $HOME/local/osmesa_22.3.7/lib/x86_64-linux-gnu
```
を実行すると，生成されたライブラリを確認できます．


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

### 動作確認
以下のコマンドで動作確認が可能です．
```
$ cd ~/Work/GitHub/KVS/Example/SupportOSMesa/Hello
$ kvsmake -G
$ kvsmake
```
これにより`output_0??.bmp`が11枚出力されます．描画に問題がないことを確認したら，InSituVisの準備の項へ進んで下さい．


# InSituVisの準備
InSituVisのライブラリを準備します．
``` 
$ cd ~/Work/GitHub
$ git clone https://github.com/vizlab-kobe/InSituVis.git
```
こちらはビルド等の操作は必要ありません．
