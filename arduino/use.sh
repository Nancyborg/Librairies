cd "$(dirname "$0")"

mkdir -p "$HOME/Arduino/libraries"

for lib in */; do
    ln -s "$(realpath "$lib")"  "$HOME/Arduino/libraries/"
done
