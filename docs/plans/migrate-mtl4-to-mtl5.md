# Plan: Migrate MTL4 to MTL5

## Context

MTL4 (`ext/stillwater-sc/mtl4`) requires Boost and uses the legacy `boost::numeric::mtl` namespace. MTL5 (`stillwater-sc/mtl5`) is a C++20 rewrite with zero Boost dependency, modern concepts, and a cleaner `mtl::` namespace. The include path changes from `boost/numeric/mtl/` to `mtl/`. Key types (`dense2D`, `dense_vector`, `inserter`) keep the same names under the same sub-namespaces (`mtl::mat::`, `mtl::vec::`).

## Complete inventory of changes

### 1. Git submodule swap

- Remove `ext/stillwater-sc/mtl4` submodule
- Add `ext/stillwater-sc/mtl5` submodule at `ext/stillwater-sc/mtl5`
- Update `.gitmodules`

### 2. Root `CMakeLists.txt`

| Line | Old | New |
|------|-----|-----|
| 55 | `set(MTL4EXT_INCLUDE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/mtl4ext)` | `set(MTLEXT_INCLUDE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/mtlext)` |
| 63 | `include_directories(${MTL4EXT_INCLUDE_HEADERS_DIR})` | `include_directories(${MTLEXT_INCLUDE_HEADERS_DIR})` |
| 84-89 | `if(Boost_FOUND) add_subdirectory(.../mtl4 build_mtl4)` | `add_subdirectory(${MPADAO_ROOT_DIR}/ext/stillwater-sc/mtl5 build_mtl5)` (unconditional — MTL5 has no Boost dependency) |

**Note:** Boost is still needed for `gauss_jordan.cpp` which uses `boost::multiprecision::cpp_bin_float_quad` independently of MTL. The `if(Boost_FOUND)` guard moves to `src/CMakeLists.txt` around the solvers subdirectory (already there).

### 3. Rename directory `src/mtl4ext/` -> `src/mtlext/`

Just a `git mv`. No content changes needed yet.

### 4. Source file changes — include path

Three files include `<boost/numeric/mtl/mtl.hpp>`. Change to `<mtl/mtl.hpp>`:

| File | Line |
|------|------|
| `src/mtlext/generators/matrix_generators.hpp` (after rename) | 10 |
| `src/solvers/gauss_jordan.cpp` | 16 |
| `benchmark/performance/main.cpp` | 20 |

### 5. Source file changes — API migration

**`src/mtlext/norms.hpp`** — Replace SFINAE with C++20 concepts:
- `mtl::traits::enable_if_vector<V, T>::type` -> `requires mtl::concepts::Vector<V>` with return type deduction
- `mtl::traits::enable_if_matrix<M, T>::type` -> `requires mtl::concepts::Matrix<M>`
- `mtl::RealMagnitude<T>::type` -> just use `typename V::value_type` (already doing that in the body)
- `mtl::Collection<V>::value_type` -> `typename V::value_type` (MTL5 dense types expose this directly)
- `mtl::dense_vector<T>` (shorthand) -> `mtl::vec::dense_vector<T>` in posit specializations
- `mtl::dense2D<T>` (shorthand) -> `mtl::mat::dense2D<T>` in posit specializations

**`src/mtlext/mtl_extensions.hpp`** — Update traits:
- `mtl::Collection<Matrix>::value_type` -> `typename Matrix::value_type`
- `mtl::Collection<Matrix>::size_type` -> `typename Matrix::size_type`
- `inserter<Matrix>` -> `mtl::mat::inserter<Matrix>` (already in `mtl::mat` namespace, should work)

**`src/mtlext/generators/matrix_generators.hpp`** — Same traits updates:
- `mtl::Collection<Matrix>::value_type/size_type` -> direct member typedefs
- `mtl::mat::inserter<Matrix>` stays the same
- `mtl::dense_vector<T>` -> `mtl::vec::dense_vector<T>`
- `mtl::mat::qr(A, Q, R)` -> verify it exists in MTL5 (it should)

**`src/mtlext/utils/print_utils.hpp`** — Light touch:
- `using namespace mtl;` then `num_rows(M)` -> should still work with MTL5's free functions
- `mtl::mat::num_rows()` / `mtl::mat::num_cols()` -> same in MTL5

**`src/solvers/gauss_jordan.cpp`**:
- `mtl::Collection<Matrix>::value_type` -> `typename Matrix::value_type`
- `using namespace mtl;` -> still works
- `mtl::vec::dense_vector<T>`, `mtl::mat::dense2D<T>` -> same names in MTL5
- `#include <mtl_extensions.hpp>` -> `#include <mtl_extensions.hpp>` (unchanged, found via include path)

**`benchmark/performance/main.cpp`**:
- Change include from `<boost/numeric/mtl/mtl.hpp>` to `<mtl/mtl.hpp>`
- No actual `mtl::` usage — the include may even be unnecessary, but keep it for now

### 6. CMakeLists.txt include path updates

| File | Old | New |
|------|-----|-----|
| `src/solvers/CMakeLists.txt:7` | `include_directories(.../mtl4)` | `include_directories(.../mtl5/include)` |
| `benchmark/performance/CMakeLists.txt:8` | `include_directories(.../mtl4)` | `include_directories(.../mtl5/include)` |

**Note:** MTL5 headers are under `include/` not at the repo root.

### 7. `benchmark/CMakeLists.txt`

Remove the `if(Boost_FOUND)` guard around the performance subdirectory. MTL5 doesn't need Boost, so the performance benchmark can always be built. Update the skip message.

### 8. Documentation updates

- **`CLAUDE.md`**: Replace all "mtl4" references with "mtl5", update description
- **`README.md`**: Update MTL4 references
- **`docs/docker-development.md`**: Update any MTL references if present

### 9. CI: `.github/workflows/cmake.yml`

Review — if Boost was only needed for MTL4, we may be able to simplify. But since `gauss_jordan.cpp` still uses `boost::multiprecision`, Boost stays.

## Execution order

1. `git submodule deinit ext/stillwater-sc/mtl4` + remove + add mtl5
2. `git mv src/mtl4ext src/mtlext`
3. Update root `CMakeLists.txt`
4. Update `src/solvers/CMakeLists.txt` and `benchmark/performance/CMakeLists.txt`
5. Update `benchmark/CMakeLists.txt` (remove Boost guard for performance)
6. Update all `#include <boost/numeric/mtl/mtl.hpp>` -> `#include <mtl/mtl.hpp>`
7. Migrate API: `Collection<T>::value_type` -> `typename T::value_type`, SFINAE -> concepts
8. Update documentation (CLAUDE.md, README.md)
9. Build and test

## Verification

```bash
mkdir build && cd build
cmake ..                    # Should configure without Boost for MTL5
make -j$(nproc)             # All targets build
ctest                       # All tests pass
cmake -DCMAKE_BUILD_TYPE=Debug ..  # Debug build works too
```

Also verify:
- `gauss_jordan` (solvers) still builds when Boost IS available
- Performance benchmark builds without Boost
- CI passes on all platforms
