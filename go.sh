#!/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd "${SCRIPT_DIR}"

for compiler_name in g++ clang++; do
	for linker_name in bfd lld; do
		build_dir="build/${compiler_name}-${linker_name}"
		rm -rf "${build_dir}"
		mkdir -p "${build_dir}"
		pushd "${build_dir}"
		cmake \
			-DCMAKE_COLOR_MAKEFILE=ON \
			-DCMAKE_VERBOSE_MAKEFILE=OFF \
			-DCMAKE_BUILD_TYPE=Debug \
			-DCMAKE_CXX_COMPILER="${compiler_name}" \
			-DCMAKE_MODULE_LINKER_FLAGS_DEBUG="-rdynamic -Wl,--export-dynamic" \
			-DCMAKE_MODULE_LINKER_FLAGS="-Wl,--enable-new-dtags -Wl,--as-needed -Wl,-z,defs -Wl,--build-id -fuse-ld=${linker_name}" \
			../..
		make
		popd
	done
done
