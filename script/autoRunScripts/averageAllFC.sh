sum=0;
count=0;

for f in ${1}/*_fsim.*;
do 
  x=$(cat ${f} | grep "fault coverage" | sed "s/fault coverage//g" | sed "s/#//g" | sed "s/ //g" | sed "s/%//g");
  echo -e "${f} with FC =  ${x}";
  sum=$(echo "${sum} + ${x} " | bc);
  count=$(echo "${count}+1" | bc);
done

averageFC=$(echo "${sum}/${count}" | bc -l ) ;
echo -e "average FC of ${count} cases is ${averageFC}\n"