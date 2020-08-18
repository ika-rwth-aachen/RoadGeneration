for file in test/*.xml
do
  echo "$file"
 ./road-generation "$file"
done
