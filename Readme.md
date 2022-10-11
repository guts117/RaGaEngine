Linux Instructions:
We are using CMake as the build system along with the Ninja generator.
* CMake can be installed as per [these](https://cmake.org/install/) instructions
* Ninja can be installed [with a package manager](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages) or [from a binary](https://github.com/ninja-build/ninja/releases)
NOTE: that both have their sources available and can be installed in that way as well, if you so desire.

To build everything simply run the `./buildall.sh` script. You may need to add execution permissions with `chmod +x buildall.sh`.
To run the built program a convenience script `./run.sh` has been made.

操作方法.txtをお読みください。
プロジェクトのビルドが出来なかった場合は、Sampleフォルダの中の.exeを実行してください。

新規実装ーーーーーーーーーーーー
-２D流体シミュレーション
-RaymarchingとSDFをしようして、３Dテキスチャーのの中での3D流体シミュレーション

参考資料：

2D 
- GPU GEMS CHAPTER 38: 
https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-38-fast-fluid-dynamics-simulation-gpu
- 2DFluidSimulation(2DシムとRK方式の参考)
https://github.com/cgurps/2DFluidSimulation

自分の追加の部分：
- RKに必要なヴェクターフィールドの情報をtextureArrayを使用して保持。
- 最大のヴェクターを持つセールの計算方法。
=======================================================================================================================================

3D
- GPU GEMS 3 CHAPTER 30:
https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-30-real-time-simulation-and-rendering-3d-fluids
- FLuid Simulation
https://www.cs.ubc.ca/~rbridson/fluidsimulation/fluids_notes.pdf

自分の追加の部分：
- GPU　Gem　3　では、AABBボリュームの計算には、一つのPrePassが必要でした。
 今回は３Dキューブを作成して、そのキューブのローカルスペースで、３DTextureをサンプルしています。
 この方法だと、一つのシェーダーパスが要らなくなり、開始点がキューブの補間された頂点になって、Raymarchingが早く終了することが可能です。
- 2DでやったRK方式を使用して、ヴェクターフィールドの計算
- Computeシェーダを使用したシミュレーション。

=======================================================================================================================================

SDF + Raymarching 
- https://www.youtube.com/watch?v=62-pRVZuS5c&ab_channel=InigoQuilez
- https://iquilezles.org/articles/distfunctions/


その他
Runge Kutta Method:
https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
https://byjus.com/maths/runge-kutta-rk4-method/
