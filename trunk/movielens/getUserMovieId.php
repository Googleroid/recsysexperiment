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
//userid ����Ҫת����ֻת��itemId
//var_dump(unserialize(file_get_contents(movieIds)));
//var_dump(unserialize(file_get_contents(userIds));
//die;
$filename = "../ml1M_data/ratings.dat";

//��һ���ǲ���Ӧ�ø����еľƵ�����е��û����Ϻţ�������Ӧ��������
//�����оƵ��ţ������е��û���ţ���ŵĹ�����������ļ� movieIds �� userIds
//(1)���Ƶ���
$movieIds = array();
$userIds = array();

$itemNum = 1;
//�����ÿһ���û���ţ��ȶ�ȡ�ļ����ݣ�Ȼ����������ʽƥ��
$rateContent = file_get_contents($filename);
$rateArray = explode("\n",$rateContent);
foreach($rateArray as $rate)
{
	$rateDetail = explode("::",$rate);
	if(count($rateDetail) < 3) continue;
	$user = $rateDetail[0];
	$item = $rateDetail[1];
	if(!isset($movieIds[$item]))
	{
		$movieIds[$item] = $itemNum; //���û����
		++$itemNum;
	}
}

$hotelIdStr = '';
foreach($movieIds as $key=>$movieId) {  //��hotelId�����c���׶�ȡ����ʽ
	$hotelIdStr .= $key."\t".($movieId)."\n";
}
//$hotelIdStr = serialize($movieIds);
//$userIdStr  = serialize($userIds);
file_put_contents('movieIds',$hotelIdStr);
file_put_contents('movieIdArrays',serialize($movieIds));
?>