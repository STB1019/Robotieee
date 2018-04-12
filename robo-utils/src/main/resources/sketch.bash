#!/bin/bash

source_folder=$1
sketch_folder=$2
library_name=$3

output_folder=${sketch_folder}

echo "source folder is `realpath ${source_folder}`"
echo "sketch folder is `realpath ${sketch_folder}`"
echo "library name is ${library_name}"
echo "output folder is `realpath ${output_folder}`"

building_zip=${library_name}
mkdir -pv ${building_zip}

#create library subfolder
mkdir -pv ${output_folder}

for hpp_file in `find ${source_folder} -name '*.hpp'`
do
	base_name=`basename --suffix=".hpp" ${hpp_file}`
	cp -v ${hpp_file} "${building_zip}/${base_name}.hpp"
done

for cpp_file in `find ${source_folder} -name '*.cpp'`
do
	base_name=`basename --suffix=".cpp" ${cpp_file}`
	cp -v ${cpp_file} "${building_zip}/${base_name}.cpp"
done

zip -r "${building_zip}.zip" "${building_zip}"
#cp -v "${building_zip}.zip" "${output_folder}"
cp -rv "${building_zip}" "${output_folder}"
echo "library files have been copied into ${output_folder}. DONE"
