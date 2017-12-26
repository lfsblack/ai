#! /usr/bin/env bash

set -e

rawdata_dir=raw_data
data_dir=data

ft_in=${data_dir}/classifier_only_article.in
python classifier.py \
    --input_article_tags_file ${rawdata_dir}/article_tags.csv \
    --input_tag_info_file ${rawdata_dir}/taginfo.csv \
    --min_labels 1 \
    --sort_tags true \
    --output_info ${ft_in} \
    --output_label_dict_file ${ft_in}.classinfo.cms \
    --output_classmap_file ${ft_in}.classmap

cat ${rawdata_dir}/only_article_class1.info ${rawdata_dir}/only_article_class2.info > ${ft_in}.classinfo


shuf -o ${ft_in}.shuf ${ft_in}


# fasttext
minCount=10
minn=0
maxn=0
thread=47
dim=100
ws=15
epoch=5
neg=5
lr=0.025

input=${ft_in}.shuf

utils/fasttext supervised \
    -input ${input} \
    -output ${input} \
    -lr ${lr} \
    -dim ${dim} \
    -ws ${ws} \
    -epoch ${epoch} \
    -minCount ${minCount} \
    -neg ${neg} \
    -loss ns \
    -bucket 2000000 \
    -minn ${minn} \
    -maxn ${maxn} \
    -thread ${thread} \
    -t 1e-4 \
    -lrUpdateRate 100  \
    && awk 'NR>2{print $1}' ${input}.vec > ${input}.dict

# cd ./generate_synonyms/
# mkdir -p build
# cd build
# cmake ..
# make
# cd ../../
./generate_synonyms/build/src/generate_synonyms \
    ${input}.bin \
    ${input}.dict \
    ${input}.classindex