Codebase and experiement scripts for this submission, where
* `src` contains the source codes and CMake list,
* `exp` contains the experiment scripts,
* `dat` contains:
    * a sample dataset;
    * the script to generate the dataset for the compiled program.


To reproduce the experiments:
* Compile the source code with the following command:
    ```bash
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ../src
    cmake --build .
    ```
* Run the corresponding experiment using the scripts in `exp`:
    * To reproduce the update efficiency experiment, run `./update.sh`;
    * To reproduce the query efficiency experiment, run `./query.sh`;
    * To reproduce the memory footprint efficiency experiment, run `./index_size.sh`;
* The experimental data will be written to the directory specified in the scripts, which is `damp` by default.