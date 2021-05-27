# Non Uniform Signed Distance Octree Generator
This repo aims to implement [Hierarchical hp-Adaptive Signed Distance Fields](http://interactive-graphics.de/index.php/research/98-hierarchical-hp-adaptive-signed-distance-fields). It extends the presented algorithm with GPU implementations, adds LSQ polynomial fitting and an interactive GUI.

## Download

 - Download the [DragonflyPack.zip](https://drive.google.com/file/d/1YHrIyQqoi5fef00poe038AW2M7P9rVfB/view?usp=sharing)
 - Find a location and create a virtual T drive, eg.: `subst T: C:\`
 - Unzip the `DragonflyPack` there. There should be 4 folders in `T:\DragonflyPack`
 - Download Dragonfly as a submodule within the cloned project (`--recursive`)
 - Use Visual Studio to build the solution

## Common use cases

### Generating new octree
<img src="https://i.imgur.com/mLZXLvq.png" width="500"/>

First set up the desired configuration with the sliders and checkboxes.
1. Start degree: the degree of the initial polynomials in the octree.
2. Max degree: the degree that the polynomials can reach during octree generation.
3. Max level: the deepest level the octree can reach during generation. (Uppermost is 0).
4. Error threshold: the maximum the error described in the [Hierarchical hp-Adaptive Signed Distance Fields](http://interactive-graphics.de/index.php/research/98-hierarchical-hp-adaptive-signed-distance-fields) can reach. The lower, the more refined and deep the octree is. 
5. Use h-adapt: use cell subdivision during generation process.
6. Use p-adapt: use polynomial degree incremention during generation process.
7. ApproxType: what kind of approximation to use
    * Gauss quadrature: the method described in the paper
    * LSQ: method based on least-squares
    * Gauss GPU: the method described in the paper implemented on the GPU
    * LSQ: method based on least-squares implemented on the GPU
8. Dropwon: which SDF to use (currently set to Sphere on picture)
9. The inputs after this are SDF specific and change based on the dropdown setting.

Second, press the Recalculate octree button and wait for the algrithm to finish.

### Switching between generated octrees
<img src="https://i.imgur.com/aOn1DHk.png" width="500"/>

There is a list of octrees that are loaded into memory. Switching between them comes at a very low cost. Simply:
1. Choose the desired octree from the list, thus making it highlighted.
2. Click Load selected. This changed the currently loaded octree, and inserts the previous one into the list.

### Load and sace octree to/from disk
<img src="https://i.imgur.com/AdkKBpV.png" width="500"/>

Click either the Load SDF or the Save SDF button. A file explorer comes up, where the desired file/location may be chosen.

### Run performance tests
Check the Evaluation wiki page.

# Dragonfly

## More Information

 - [Dragonfly Poster](https://people.inf.elte.hu/csabix/publications/articles/WSPS_2020_Poster_Dragonfly.pdf)
 - [Dragonfly Project](https://github.com/ELTE-IK-CG/Dragonfly)
 - Or contact the authors at csabix@inf.elte.hu or bundas@inf.elte.hu

## Credits

Dragonfly is developed by Csaba Bálint and Róbert Bán at Eötvös Loránd University, Budapest, Hungary.

Supported by the ÚNKP19-3 New National Excellence Program of the Ministry for Innovation and Technology. The project has been supported by the European Union, co-financed by the European Social Fund (EFOP-3.6.3-VEKOP-16-2017-00001).

Dragonfly is licensed under the MIT License, see `Dragonfly\LICENSE.txt` for more information.
