<?php
define('DS',DIRECTORY_SEPARATOR);
$dirBase = '../tripDataset';
$newDirBase = '../tripDataset_t';
$hotelIds = unserialize(file_get_contents('hotelIdArrays'));
$userIds  = unserialize(file_get_contents('userIdArrays'));

//���ÿһ���ļ������滻,Ȼ���������һ���µ��ļ�
foreach( glob($dirBase.'/*.txt') as $filename)
{
	//���ÿһ���ļ�����ȡ���Ƶ��id
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);
	$hotelNumId = $hotelIds[$hotelId];
	
	//�����ÿһ���û���ţ��ȶ�ȡ�ļ����ݣ�Ȼ�����µ��ļ�����ʱ����ʱ������Ӱ��
	$newFileContent = '';
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)\n.+\n.+\n<Rating>(.+)\n/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $key=>$user)
	{
		if('A TripAdvisor Member' == $user)continue;
		$userNumId = $userIds[$user];
		//var_dump($user,$userNumId);
		$newFileContent .= $userNumId.':'.$usersArray[2][$key]."\n";
	}
	//echo $newFileContent;die;
	file_put_contents($newDirBase.DS.$hotelNumId, $newFileContent);
}
?>