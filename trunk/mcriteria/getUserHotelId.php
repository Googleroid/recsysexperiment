<?php
//var_dump(unserialize(file_get_contents(hotelIds)));
//var_dump(unserialize(file_get_contents(userIds));
//die;
$dirBase = '../tripDataset';

//��һ���ǲ���Ӧ�ø����еľƵ�����е��û����Ϻţ�������Ӧ��������
//�����оƵ��ţ������е��û���ţ���ŵĹ�����������ļ� hotelIds �� userIds
//(1)���Ƶ���
$hotelIds = array();
$userIds = array();
$num = 1;
$userNum = 1;
foreach( glob($dirBase.'/*.txt') as $filename)
{
	//���ÿһ���ļ�����ȡ���Ƶ��id
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);
	$hotelIds[$hotelId] = $num;
	++$num;
	//�����ÿһ���û���ţ��ȶ�ȡ�ļ����ݣ�Ȼ����������ʽƥ��
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $user)
	{
		if('A TripAdvisor Member' == $user)continue;
		if(!isset($userIds[$user]))
		{
			$userIds[$user] = $userNum; //���û����
			++$userNum;
		}
	}
}
$hotelIdStr = '';
foreach($hotelIds as $key=>$hotelId) {  //��hotelId�����c���׶�ȡ����ʽ
	$hotelIdStr .= $key."\t".($hotelId)."\n";
}
$userIdStr = '';
foreach($userIds as $key=>$userId) {  //��hotelId�����c���׶�ȡ����ʽ
	$userIdStr .= $key."\t".($userId)."\n";
}
//$hotelIdStr = serialize($hotelIds);
//$userIdStr  = serialize($userIds);
file_put_contents('hotelIds',$hotelIdStr);
file_put_contents('userIds',$userIdStr);
file_put_contents('hotelIdArrays',serialize($hotelIds));
file_put_contents('userIdArrays',serialize($userIds));
?>