sum=0;
count=0;

for f in ${1}/*_fsim.*;
do 
  x=$(cat ${f} | grep "Patterns" | sed "s/#Patterns//g" | sed "s/#//g" | sed "s/ //g");
  echo -e "${f} with TL =  ${x}";
  sum=$(echo "${sum} + ${x} " | bc);
  count=$(echo "${count}+1" | bc);
done

averageFC=$(echo "${sum}/${count}" | bc -l ) ;
echo -e "average TL of ${count} cases is ${averageFC}\n"