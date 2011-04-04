<?php
/*
* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
* please maitain the copyright information completely when you redistribute the code.
* 
* Please contact me via email honglianglv@gmail.com
* my blog: http://lifecrunch.biz
* my twitter: http://twitter.com/honglianglv
*
* It is free software; you can redistribute it and/or modify it under 
* the GNU General Public License as published by the Free Software
* Foundation; either version 1, or (at your option) any later version.
*/
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
		if("A TripAdvisor Member" == $user)continue;
		$userNumId = $userIds[$user];
		if($userNumId == 13)continue;
		if($userNumId == 31)continue;
		//echo "########".$userNumId."    ".$user."     "."\n";
		//var_dump($user,$userNumId);
		$newFileContent .= $userNumId.':'.$usersArray[2][$key]."\n";
	}
	//echo $newFileContent;die;
	file_put_contents($newDirBase.DS.$hotelNumId, $newFileContent);
}
?>