#!/usr/bin/env sh
set -e

./build/tools/caffe train --solver=lanenet/train_lanenet.prototxt $@