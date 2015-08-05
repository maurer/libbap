#!/usr/bin/env bash
source setup.data
cat <<EOF
prefix=$prefix
exec_prefix=$exec_prefix
includedir=\${prefix}/include
libdir=\${exec_prefix}/lib

Name: bap
Description: C bindings to BAP
Version: 0.0.0
Cflags: -I\${includedir}
Libs: -L\${libdir} -lbap -ldl -lm
EOF
