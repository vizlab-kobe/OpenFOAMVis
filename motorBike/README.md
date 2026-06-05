<img width="512" height="512" alt="motorBike_iso" src="https://github.com/user-attachments/assets/c98ae564-1b1b-404c-93cf-728767569108" /># motorBike後流の乱流解析

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

さらに解析ディレクトリに移動して解析を実行します．
```
$ cd ~/Work/GitHub/OpenFOAMVis/motorBike/vis_motorBike
$ mpirun np 8 motorBike_pisoFoam -parallel

```
として実行します．`Output`ディレクトリに`*.bmp`ファイルが出力されればOKです．

## 描画例
<img width="512" height="512" alt="motorBike_iso" src="https://github.com/user-attachments/assets/c30b3966-a2e7-4211-974c-fc0c2e5f1df1" />
