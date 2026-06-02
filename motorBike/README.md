# motorBikeの解析

ここではOpenFOAM v2412のtutorialに含まれるmotorBikeの非定常解析のin-situ可視化を行います．走行中のバイクの後方乱流を表す流速の大きさの等値面を可視化します．


## 準備と実行コマンド

以下の操作を実行します．まずはソースコードを取得します．
```
$ cd ~/Work/GitHub
$ git clone https://github.com/vizlab-kobe/OpenFOAMVis.git
```

ソルバーディレクトリに移動してビルドをします．デフォルトではEGL版がビルドされます．
```
$ cd OpenFOAMVis/motorBike/motorBike_pisoFoam
$ wclean && wmake
```
OSMesa版をビルドしたい場合には
```
$ cd OpenFOAMVis/motorBike/motorBike_pisoFoam
$ cp Make/option_osmesa Make/option
$ wclean && wmake
```
として下さい．以上の操作でin-situ可視化に対応したmotorBike解析用の非定常非圧縮ソルバー`motorBike_pisoFoam`がビルドされます．

さらにpropeller解析ディレクトリに移動して解析を実行します．
```
$ cd ~/Work/GitHub/OpenFOAMVis/motorBike/vis_motorBike
$ mpirun np 8 motorBike_pisoFoam -parallel
```
として実行します．環境にも依りますが，計算に非常に長い時間がかかりますので，
```
$ nohup mpirun -np 8 motorBike_pisoFoam -parallel 2>&1 log.dat &
```
として
```
$ tail -f log.dat
```
で監視しても良いです．`Output`ディレクトリに`*.bmp`ファイルが出力されればOKです．