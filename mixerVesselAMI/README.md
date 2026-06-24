# motorBike後流の乱流解析

ここではOpenFOAM v2412のtutorialに含まれるmixerVesselAMIの非定常解析のin-situ可視化を行います．撹拌タンク内の気体部分の流速の大きさの等値面を可視化します．


## 準備と実行コマンド

以下の操作を実行します．まずはソースコードを取得します．
```
$ cd ~/Work/GitHub
$ git clone https://github.com/vizlab-kobe/OpenFOAMVis.git
```

ソルバーディレクトリに移動してビルドをします．デフォルトではEGL版がビルドされます．
```
$ cd OpenFOAMVis/mixerVesselAMI/mixerVesselAMI_interFoam
$ wclean && wmake
```
OSMesa版をビルドしたい場合には
```
$ cd OpenFOAMVis/motorBike/mixerVesselAMI_interFoam
$ cp Make/option_osmesa Make/option
$ wclean && wmake
```
として下さい．以上の操作でin-situ可視化に対応したmotorBike解析用の非定常非圧縮ソルバー`mixerVessel_interFoam`がビルドされます．

さらに解析ディレクトリに移動して解析を実行します．
```
$ cd ~/Work/GitHub/OpenFOAMVis/mixerVesselAMI/vis_mixerVesselAMI
$ mpirun np 6 mixerVesselAMI_interFoam -parallel

```
として実行します．`Output`ディレクトリに`*.bmp`ファイルが出力されればOKです．

## 描画例
