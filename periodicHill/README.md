# periodicHillの解析

ここではOpenFOAM v2412のtutorialに含まれるperiodicHillの非定常解析のin-situ可視化を行います．LES解析による乱流解析を可視化します．


## 準備と実行コマンド

以下の操作を実行します．まずはソースコードを取得します．
```
$ cd ~/Work/GitHub
$ git clone https://github.com/vizlab-kobe/OpenFOAMVis.git
```

ソルバーディレクトリに移動してビルドをします．デフォルトではEGL版がビルドされます．
```
$ cd OpenFOAMVis/periodicHill/periodicHill_pimpleFoam
$ wclean && wmake
```
OSMesa版をビルドしたい場合には
```
$ cd OpenFOAMVis/periodicHill/periodicHill_pimpleFoam
$ cp Make/options_osmesa Make/options
$ wclean && wmake
```
として下さい．以上の操作でin-situ可視化に対応したperiodicHill解析用の非定常非圧縮ソルバー`periodicHill_pimpleFoam`がビルドされます．

さらにperiodicHill解析ディレクトリに移動して解析を実行します．
```
$ cd ~/Work/GitHub/OpenFOAMVis/periodicHill/vis_periodicHill
$ mpirun -np 16 periodicHill_pimpleFoam -parallel
```
として実行します．

`Output`ディレクトリに`*.bmp`ファイルが出力されればOKです．


### 可視化例
<img width="512" height="512" alt="periodicHill_iso" src="https://github.com/user-attachments/assets/71cf4264-0f22-4304-8164-509759272ca2" />
