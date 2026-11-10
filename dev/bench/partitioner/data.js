window.BENCHMARK_DATA = {
  "lastUpdate": 1794317356610,
  "repoUrl": "https://github.com/CIFASIS/sb-graph",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "sansone@cifasis-conicet.gov.ar",
            "name": "Franco Sansone",
            "username": "francosansone"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "1f755f5f2ca5bdd03d382567c85ec34a6ee056fe",
          "message": "Iss 121 port external tool from sb graph partitioner (#147)\n\n* Port external tools from  sbg partitioner.\n\n* generate graph from sbg input\n\n* connecting graph partition with sbg\n\n* added memory monitor\n\n* improve compilation time\n\n* prepare benchmarking\n\n* update script\n\n* added memory benchmarking\n\n* removed annoying print\n\n* fix metis\n\n* Use references in Scotch partitioner.\n\n* added two python scripts\n\n* temporal change with many lines of code commented to test edge-cut values\n\n* added data to test\n\n* make it work for Scotch\n\n* print connections\n\n* upload air conditioner model input\n\n* release by default\n\n* take into account if air conditioner cont is the input\n\n* air cond with controllers\n\n* pass number of sections to air cond with controller\n\n* update_diff working\n\n* compute ec and ic in each step\n\n* compute all methods all at once\n\n* call graph partitioner properly\n\n* just adapting memory monitor\n\n* communication cost changes\n\n* change back function name\n\n* split sets according to their connections\n\n* update graph partitioner according to changes\n\n* disable test case\n\n* stop compiling partition_graph_cc\n\n* ignore deploy directories\n\n* add code to compute metrics\n\n* added 'dump-results' flag\n\n* added dump-results flag\n\n* duplicate N:N connections\n\n* dump results\n\n* fixed internal comm\n\n* use size_t to avoid overflow\n\n* create sb graph for advection 2d\n\n* make it work for 2-dimensional graphs\n\n* make advection 2d work\n\n* dump graph edges for 2D\n\n* make graph partitioner or 2D\n\n* remove debug messages\n\n* hack for 2d graphs\n\n* make partitioner handle two dimensions properly\n\n* dump results for 2d\n\n* write 2d graph properly\n\n* [iss-136]\nSquashed commit of the following:\n\ncommit bafa90cd2e4f72af843c8d99122ba8301182e6d3\nAuthor: joaquin.f.fernandez <joaquin.f.fernandez@gmail.com>\nDate:   Thu May 14 13:44:31 2026 -0300\n\n    Added initial example for qss simulation pipeline.\n\n* removed annoying messages\n\n* examples and simulation data for JAIIO 55\n\n* removed annotiny messages\n\n* final changes to model implementations\n\n* dup conns\n\n* add dup conns\n\n* added input generators\n\n* last changes before tag\n\n* Remove obsolete sources\n\n* Merge sb-graph-dev\n\n* make cut_interval_by_dimension work for every dimension\n\n* Revert \"make cut_interval_by_dimension work for every dimension\"\n\nThis reverts commit 5445db7f57bb90d07a84465af78d782b13274715.\n\n* Revert \"Merge sb-graph-dev\"\n\nThis reverts commit 28726463386dc9e43f808f5a85b43c8a87209b5c.\n\n* make cut_interval_by_dimension work for N dimensions\n\n* added benchmark\n\n* rmove ground truth files\n\n* remove part files\n\n---------\n\nCo-authored-by: joaquin.f.fernandez <joaquin.f.fernandez@gmail.com>",
          "timestamp": "2026-11-03T13:28:25-03:00",
          "tree_id": "c2d28e227d30a9a3f62607ec565902ee20544bfb",
          "url": "https://github.com/CIFASIS/sb-graph/commit/1f755f5f2ca5bdd03d382567c85ec34a6ee056fe"
        },
        "date": 1793723431586,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "advection2D size=1000 parts=4",
            "value": 2.0233119999999998,
            "unit": "ms",
            "extra": "build=0.0ms partition=2.0ms runs=[2.0649900000000003, 2.014682, 2.0233119999999998]"
          },
          {
            "name": "advection2D size=1000 parts=8",
            "value": 4.397048,
            "unit": "ms",
            "extra": "build=0.0ms partition=4.4ms runs=[5.318884000000001, 4.397048, 4.377527000000001]"
          },
          {
            "name": "advection2D size=1000 parts=16",
            "value": 30.206968,
            "unit": "ms",
            "extra": "build=0.0ms partition=30.2ms runs=[30.615566, 30.206968, 29.775357]"
          },
          {
            "name": "advection2D size=10000 parts=4",
            "value": 2.336239,
            "unit": "ms",
            "extra": "build=0.0ms partition=2.3ms runs=[2.016408, 2.76803, 2.336239]"
          },
          {
            "name": "advection2D size=100000 parts=4",
            "value": 12.496173,
            "unit": "ms",
            "extra": "build=0.0ms partition=12.5ms runs=[12.496173, 13.579627, 11.861917]"
          },
          {
            "name": "air_conditioners size=1000 parts=4",
            "value": 0.901022,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.6ms runs=[0.901022, 0.8177439999999999, 0.980701]"
          },
          {
            "name": "air_conditioners size=1000 parts=8",
            "value": 1.467748,
            "unit": "ms",
            "extra": "build=0.3ms partition=1.2ms runs=[1.78673, 1.467748, 1.4587819999999998]"
          },
          {
            "name": "air_conditioners size=1000 parts=16",
            "value": 16.108833999999998,
            "unit": "ms",
            "extra": "build=0.3ms partition=15.8ms runs=[15.985707, 16.108833999999998, 17.118686999999998]"
          },
          {
            "name": "air_conditioners size=10000 parts=4",
            "value": 1.078134,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.7ms runs=[1.1319540000000001, 1.078134, 1.074897]"
          },
          {
            "name": "air_conditioners size=100000 parts=4",
            "value": 1.04428,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.8ms runs=[1.158635, 1.039973, 1.04428]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=4",
            "value": 647.663491,
            "unit": "ms",
            "extra": "build=0.2ms partition=647.5ms runs=[647.663491, 642.365749, 653.081746]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=8",
            "value": 15413.25537,
            "unit": "ms",
            "extra": "build=0.3ms partition=15413.0ms runs=[15413.25537, 15340.072639999999, 15476.443476]"
          },
          {
            "name": "air_conditioners_cont size=10000 parts=4",
            "value": 573.852592,
            "unit": "ms",
            "extra": "build=0.2ms partition=573.7ms runs=[613.875688, 573.852592, 565.82513]"
          },
          {
            "name": "air_conditioners_cont size=100000 parts=4",
            "value": 567.884836,
            "unit": "ms",
            "extra": "build=0.2ms partition=567.7ms runs=[567.884836, 567.407933, 577.069478]"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "committer": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "distinct": true,
          "id": "480ae3306722fba542f14fe345b2d1625da11fa5",
          "message": "[iss-144]\n\n*<merge> origin/sb-graph-dev -> iss-144-new-topological-sorting\n\n*<feat>[Sorting]: added two phased implementation\n\n*<feat>[MinVertexTS]: delete processed repetition\n\n*<build>: improved flags for debug mode\n\n*<test>: corrected descriptions of tests\n\n*<refactor>: added sort for PWMap\n\n*<merge> origin/sb-graph-dev -> iss-144-new-topological-sorting\n\n*<feat>[Map]: added function that sorts a set",
          "timestamp": "2026-11-05T11:27:05-03:00",
          "tree_id": "ed810f340426fb1f648cef2fd8bac2f6b2f266a3",
          "url": "https://github.com/CIFASIS/sb-graph/commit/480ae3306722fba542f14fe345b2d1625da11fa5"
        },
        "date": 1793888979055,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "advection2D size=1000 parts=4",
            "value": 1.2460010000000001,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.2ms runs=[1.2460010000000001, 1.2895560000000001, 1.22384]"
          },
          {
            "name": "advection2D size=1000 parts=8",
            "value": 2.559819,
            "unit": "ms",
            "extra": "build=0.0ms partition=2.5ms runs=[2.554576, 2.66006, 2.559819]"
          },
          {
            "name": "advection2D size=1000 parts=16",
            "value": 17.40547,
            "unit": "ms",
            "extra": "build=0.0ms partition=17.4ms runs=[17.40547, 17.4218, 16.873057]"
          },
          {
            "name": "advection2D size=10000 parts=4",
            "value": 1.222521,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.2ms runs=[1.254183, 1.222521, 1.175044]"
          },
          {
            "name": "advection2D size=100000 parts=4",
            "value": 6.7034530000000006,
            "unit": "ms",
            "extra": "build=0.0ms partition=6.7ms runs=[6.758668, 6.696122000000001, 6.7034530000000006]"
          },
          {
            "name": "air_conditioners size=1000 parts=4",
            "value": 0.6186,
            "unit": "ms",
            "extra": "build=0.2ms partition=0.4ms runs=[0.6186, 0.569715, 0.697191]"
          },
          {
            "name": "air_conditioners size=1000 parts=8",
            "value": 1.0232459999999999,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.7ms runs=[1.0593, 1.015523, 1.0232459999999999]"
          },
          {
            "name": "air_conditioners size=1000 parts=16",
            "value": 9.386223,
            "unit": "ms",
            "extra": "build=0.4ms partition=8.9ms runs=[9.386223, 9.7897, 9.302522999999999]"
          },
          {
            "name": "air_conditioners size=10000 parts=4",
            "value": 0.821969,
            "unit": "ms",
            "extra": "build=0.4ms partition=0.4ms runs=[0.7945679999999999, 0.821969, 0.840678]"
          },
          {
            "name": "air_conditioners size=100000 parts=4",
            "value": 0.788859,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.4ms runs=[0.788859, 1.049655, 0.745493]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=4",
            "value": 368.78596899999997,
            "unit": "ms",
            "extra": "build=0.1ms partition=368.7ms runs=[375.050006, 368.78596899999997, 362.116215]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=8",
            "value": 8631.568733,
            "unit": "ms",
            "extra": "build=0.2ms partition=8631.4ms runs=[8740.047095, 8631.568733, 8587.977296000001]"
          },
          {
            "name": "air_conditioners_cont size=10000 parts=4",
            "value": 309.46779100000003,
            "unit": "ms",
            "extra": "build=0.1ms partition=309.3ms runs=[310.325049, 308.632645, 309.46779100000003]"
          },
          {
            "name": "air_conditioners_cont size=100000 parts=4",
            "value": 311.821882,
            "unit": "ms",
            "extra": "build=0.1ms partition=311.7ms runs=[313.87926500000003, 311.821882, 307.518619]"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "committer": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "distinct": true,
          "id": "b8efe88914ae7ebff297e51b880105e0be9dd313",
          "message": "<fix>: fixed MDI sort generation",
          "timestamp": "2026-11-06T19:04:14-03:00",
          "tree_id": "facecfbe889f818850ef35ef503ec40c2f514fc5",
          "url": "https://github.com/CIFASIS/sb-graph/commit/b8efe88914ae7ebff297e51b880105e0be9dd313"
        },
        "date": 1794002769515,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "advection2D size=1000 parts=4",
            "value": 1.622611,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.6ms runs=[1.660758, 1.622611, 1.5954130000000002]"
          },
          {
            "name": "advection2D size=1000 parts=8",
            "value": 3.441422,
            "unit": "ms",
            "extra": "build=0.0ms partition=3.4ms runs=[3.414174, 3.503622, 3.441422]"
          },
          {
            "name": "advection2D size=1000 parts=16",
            "value": 23.602402,
            "unit": "ms",
            "extra": "build=0.0ms partition=23.6ms runs=[23.602402, 23.646226, 23.545913]"
          },
          {
            "name": "advection2D size=10000 parts=4",
            "value": 1.5825250000000002,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.6ms runs=[1.579234, 1.5825250000000002, 1.5927529999999999]"
          },
          {
            "name": "advection2D size=100000 parts=4",
            "value": 9.392356,
            "unit": "ms",
            "extra": "build=0.0ms partition=9.4ms runs=[9.392356, 9.342813, 9.464057]"
          },
          {
            "name": "air_conditioners size=1000 parts=4",
            "value": 0.720093,
            "unit": "ms",
            "extra": "build=0.2ms partition=0.5ms runs=[0.720093, 0.6944440000000001, 0.849457]"
          },
          {
            "name": "air_conditioners size=1000 parts=8",
            "value": 1.2322929999999999,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.9ms runs=[1.2322929999999999, 1.262889, 1.216439]"
          },
          {
            "name": "air_conditioners size=1000 parts=16",
            "value": 13.440132,
            "unit": "ms",
            "extra": "build=0.3ms partition=13.1ms runs=[18.998226000000003, 13.440132, 12.803166000000001]"
          },
          {
            "name": "air_conditioners size=10000 parts=4",
            "value": 0.833514,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.5ms runs=[0.853073, 0.833514, 0.819933]"
          },
          {
            "name": "air_conditioners size=100000 parts=4",
            "value": 0.811742,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.5ms runs=[0.805161, 0.815797, 0.811742]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=4",
            "value": 522.6436699999999,
            "unit": "ms",
            "extra": "build=0.1ms partition=522.5ms runs=[522.6436699999999, 516.359156, 547.571494]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=8",
            "value": 12096.320151,
            "unit": "ms",
            "extra": "build=0.2ms partition=12096.1ms runs=[11957.120792, 12222.610346, 12096.320151]"
          },
          {
            "name": "air_conditioners_cont size=10000 parts=4",
            "value": 439.77964,
            "unit": "ms",
            "extra": "build=0.1ms partition=439.6ms runs=[439.77964, 450.33913099999995, 438.41191299999997]"
          },
          {
            "name": "air_conditioners_cont size=100000 parts=4",
            "value": 440.976846,
            "unit": "ms",
            "extra": "build=0.1ms partition=440.8ms runs=[424.41472699999997, 444.15695200000005, 440.976846]"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "committer": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "distinct": true,
          "id": "b8efe88914ae7ebff297e51b880105e0be9dd313",
          "message": "<fix>: fixed MDI sort generation",
          "timestamp": "2026-11-06T19:04:14-03:00",
          "tree_id": "facecfbe889f818850ef35ef503ec40c2f514fc5",
          "url": "https://github.com/CIFASIS/sb-graph/commit/b8efe88914ae7ebff297e51b880105e0be9dd313"
        },
        "date": 1794004418306,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "advection2D size=1000 parts=4",
            "value": 1.56815,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.5ms runs=[1.615782, 1.56815, 1.557856]"
          },
          {
            "name": "advection2D size=1000 parts=8",
            "value": 3.445037,
            "unit": "ms",
            "extra": "build=0.0ms partition=3.4ms runs=[3.445037, 3.51979, 3.43745]"
          },
          {
            "name": "advection2D size=1000 parts=16",
            "value": 23.673627,
            "unit": "ms",
            "extra": "build=0.0ms partition=23.6ms runs=[22.947733, 23.673627, 23.948113]"
          },
          {
            "name": "advection2D size=10000 parts=4",
            "value": 1.603384,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.6ms runs=[1.603384, 1.5771469999999999, 1.606996]"
          },
          {
            "name": "advection2D size=100000 parts=4",
            "value": 9.265462999999999,
            "unit": "ms",
            "extra": "build=0.0ms partition=9.2ms runs=[9.286510999999999, 9.264032, 9.265462999999999]"
          },
          {
            "name": "air_conditioners size=1000 parts=4",
            "value": 0.775576,
            "unit": "ms",
            "extra": "build=0.2ms partition=0.5ms runs=[0.775576, 0.666351, 0.832511]"
          },
          {
            "name": "air_conditioners size=1000 parts=8",
            "value": 1.23868,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.9ms runs=[1.282267, 1.23868, 1.228516]"
          },
          {
            "name": "air_conditioners size=1000 parts=16",
            "value": 18.04047,
            "unit": "ms",
            "extra": "build=5.6ms partition=12.3ms runs=[12.371675, 25.6656, 18.04047]"
          },
          {
            "name": "air_conditioners size=10000 parts=4",
            "value": 0.859271,
            "unit": "ms",
            "extra": "build=0.4ms partition=0.5ms runs=[8.146215999999999, 0.859271, 0.799482]"
          },
          {
            "name": "air_conditioners size=100000 parts=4",
            "value": 0.824989,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.5ms runs=[0.831218, 0.792611, 0.824989]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=4",
            "value": 511.555255,
            "unit": "ms",
            "extra": "build=0.1ms partition=511.4ms runs=[511.555255, 512.8056349999999, 508.98301100000003]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=8",
            "value": 12270.730376,
            "unit": "ms",
            "extra": "build=0.2ms partition=12270.5ms runs=[12124.614823, 12393.009624999999, 12270.730376]"
          },
          {
            "name": "air_conditioners_cont size=10000 parts=4",
            "value": 440.788143,
            "unit": "ms",
            "extra": "build=0.1ms partition=440.6ms runs=[440.788143, 446.22768099999996, 438.981875]"
          },
          {
            "name": "air_conditioners_cont size=100000 parts=4",
            "value": 451.659237,
            "unit": "ms",
            "extra": "build=0.1ms partition=451.5ms runs=[447.172825, 451.659237, 459.22662299999996]"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "committer": {
            "email": "denise.marzorati@gmail.com",
            "name": "kalashnikovni",
            "username": "Kalashnikovni"
          },
          "distinct": true,
          "id": "b8efe88914ae7ebff297e51b880105e0be9dd313",
          "message": "<fix>: fixed MDI sort generation",
          "timestamp": "2026-11-06T19:04:14-03:00",
          "tree_id": "facecfbe889f818850ef35ef503ec40c2f514fc5",
          "url": "https://github.com/CIFASIS/sb-graph/commit/b8efe88914ae7ebff297e51b880105e0be9dd313"
        },
        "date": 1794316871333,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "advection2D size=1000 parts=4",
            "value": 1.9933029999999998,
            "unit": "ms",
            "extra": "build=0.0ms partition=2.0ms runs=[1.9933029999999998, 2.001252, 1.986707]"
          },
          {
            "name": "advection2D size=1000 parts=8",
            "value": 4.355016,
            "unit": "ms",
            "extra": "build=0.0ms partition=4.3ms runs=[4.333724999999999, 4.901542, 4.355016]"
          },
          {
            "name": "advection2D size=1000 parts=16",
            "value": 30.119742,
            "unit": "ms",
            "extra": "build=0.0ms partition=30.1ms runs=[30.119742, 29.684342, 30.608966]"
          },
          {
            "name": "advection2D size=10000 parts=4",
            "value": 2.090546,
            "unit": "ms",
            "extra": "build=0.0ms partition=2.0ms runs=[2.3333060000000003, 2.06481, 2.090546]"
          },
          {
            "name": "advection2D size=100000 parts=4",
            "value": 11.722064,
            "unit": "ms",
            "extra": "build=0.0ms partition=11.7ms runs=[11.875478, 11.639885999999999, 11.722064]"
          },
          {
            "name": "air_conditioners size=1000 parts=4",
            "value": 0.859513,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.6ms runs=[0.859513, 0.829404, 2.829237]"
          },
          {
            "name": "air_conditioners size=1000 parts=8",
            "value": 1.493423,
            "unit": "ms",
            "extra": "build=0.4ms partition=1.1ms runs=[1.493423, 1.465095, 1.507342]"
          },
          {
            "name": "air_conditioners size=1000 parts=16",
            "value": 15.604351,
            "unit": "ms",
            "extra": "build=0.4ms partition=15.2ms runs=[15.518239, 16.129911, 15.604351]"
          },
          {
            "name": "air_conditioners size=10000 parts=4",
            "value": 0.9909020000000001,
            "unit": "ms",
            "extra": "build=0.4ms partition=0.6ms runs=[1.053429, 0.973317, 0.9909020000000001]"
          },
          {
            "name": "air_conditioners size=100000 parts=4",
            "value": 0.9884459999999999,
            "unit": "ms",
            "extra": "build=0.4ms partition=0.6ms runs=[0.9884459999999999, 1.058802, 0.986399]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=4",
            "value": 626.7514000000001,
            "unit": "ms",
            "extra": "build=0.2ms partition=626.6ms runs=[635.835245, 626.7514000000001, 625.644144]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=8",
            "value": 14358.554344999999,
            "unit": "ms",
            "extra": "build=0.3ms partition=14358.3ms runs=[14504.041645, 14342.759414, 14358.554344999999]"
          },
          {
            "name": "air_conditioners_cont size=10000 parts=4",
            "value": 538.73321,
            "unit": "ms",
            "extra": "build=0.2ms partition=538.6ms runs=[533.856011, 541.036746, 538.73321]"
          },
          {
            "name": "air_conditioners_cont size=100000 parts=4",
            "value": 534.764676,
            "unit": "ms",
            "extra": "build=0.2ms partition=534.6ms runs=[536.862935, 533.842075, 534.764676]"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "denise.marzorati@gmail.com",
            "name": "Denise M",
            "username": "Kalashnikovni"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "5b1384bee67f546ab6f4777fe4ecc5440fa04d30",
          "message": "<ci>: change alert handling in partitioner benchmark",
          "timestamp": "2026-11-10T10:25:05-03:00",
          "tree_id": "68fccdc1251347e8daee117427fe7c60f5b13bcd",
          "url": "https://github.com/CIFASIS/sb-graph/commit/5b1384bee67f546ab6f4777fe4ecc5440fa04d30"
        },
        "date": 1794317355769,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "advection2D size=1000 parts=4",
            "value": 1.6073060000000001,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.6ms runs=[1.67694, 1.569303, 1.6073060000000001]"
          },
          {
            "name": "advection2D size=1000 parts=8",
            "value": 3.4870349999999997,
            "unit": "ms",
            "extra": "build=0.0ms partition=3.5ms runs=[3.4870349999999997, 3.476465, 3.787943]"
          },
          {
            "name": "advection2D size=1000 parts=16",
            "value": 23.746948999999997,
            "unit": "ms",
            "extra": "build=0.0ms partition=23.7ms runs=[23.598003000000002, 23.946377000000002, 23.746948999999997]"
          },
          {
            "name": "advection2D size=10000 parts=4",
            "value": 1.599593,
            "unit": "ms",
            "extra": "build=0.0ms partition=1.6ms runs=[1.5982159999999999, 1.6062429999999999, 1.599593]"
          },
          {
            "name": "advection2D size=100000 parts=4",
            "value": 9.334557,
            "unit": "ms",
            "extra": "build=0.0ms partition=9.3ms runs=[9.617915, 9.263304999999999, 9.334557]"
          },
          {
            "name": "air_conditioners size=1000 parts=4",
            "value": 0.7321200000000001,
            "unit": "ms",
            "extra": "build=0.2ms partition=0.5ms runs=[0.7321200000000001, 0.7132419999999999, 3.6464540000000003]"
          },
          {
            "name": "air_conditioners size=1000 parts=8",
            "value": 1.2512409999999998,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.9ms runs=[8.773457, 1.237721, 1.2512409999999998]"
          },
          {
            "name": "air_conditioners size=1000 parts=16",
            "value": 12.673527,
            "unit": "ms",
            "extra": "build=0.4ms partition=12.3ms runs=[12.644793, 12.673527, 14.194606]"
          },
          {
            "name": "air_conditioners size=10000 parts=4",
            "value": 0.821586,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.5ms runs=[0.887304, 0.821586, 0.8185]"
          },
          {
            "name": "air_conditioners size=100000 parts=4",
            "value": 0.8346549999999999,
            "unit": "ms",
            "extra": "build=0.3ms partition=0.5ms runs=[0.8346549999999999, 0.848576, 0.8060620000000001]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=4",
            "value": 537.230404,
            "unit": "ms",
            "extra": "build=0.1ms partition=537.1ms runs=[537.230404, 511.205847, 546.515982]"
          },
          {
            "name": "air_conditioners_cont size=1000 parts=8",
            "value": 12405.114593,
            "unit": "ms",
            "extra": "build=0.2ms partition=12404.9ms runs=[12405.114593, 12049.316335, 12580.117006999999]"
          },
          {
            "name": "air_conditioners_cont size=10000 parts=4",
            "value": 441.115954,
            "unit": "ms",
            "extra": "build=0.1ms partition=441.0ms runs=[441.115954, 436.201468, 442.755974]"
          },
          {
            "name": "air_conditioners_cont size=100000 parts=4",
            "value": 447.90386,
            "unit": "ms",
            "extra": "build=0.1ms partition=447.8ms runs=[450.815715, 447.375381, 447.90386]"
          }
        ]
      }
    ]
  }
}