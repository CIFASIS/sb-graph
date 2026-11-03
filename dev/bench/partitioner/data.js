window.BENCHMARK_DATA = {
  "lastUpdate": 1793723432749,
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
      }
    ]
  }
}