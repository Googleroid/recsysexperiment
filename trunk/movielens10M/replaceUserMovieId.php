<?php

//�滻movieId��user����Ҫ��
define('DS',DIRECTORY_SEPARATOR);

//var_dump(unserialize(file_get_contents(userIds));
//die;
$filename = "../ml10M_data/ratings.dat";
$dstFilename = "../ml10M_data/ratings_t.dat";
$movieIds = unserialize(file_get_contents("movieIdArrays"));

//�����ÿһ���û���ţ��ȶ�ȡ�ļ����ݣ�Ȼ����������ʽƥ��
$rateContent = file_get_contents($filename);
$rateArray = explode("\n",$rateContent);
$ret = '';
foreach($rateArray as $rate)
{
	$rateDetail = explode("::",$rate);
	if(count($rateDetail) < 3) continue;
	$user = $rateDetail[0];
	$item = $movieIds[$rateDetail[1]];
	$rate = $rateDetail[2];
	$timestamp = $rateDetail[3];
	$ret .= $user."::".$item."::".$rate."::".$timestamp."\n";
}
file_put_contents($dstFilename,$ret);
?>