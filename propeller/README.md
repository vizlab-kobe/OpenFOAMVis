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




