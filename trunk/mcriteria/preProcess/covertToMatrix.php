<?php
/*
 * convert the ratings in the files to 8 matrixs
 * 
 */
$dirBase = 'F:\dataset\tripadvisor\Texts';

//��һ���ǲ���Ӧ�ø����еľƵ�����е��û����Ϻţ�������Ӧ��������
//�����оƵ��ţ������е��û���ţ���ŵĹ�����������ļ� hotelIds �� userIds
//(1)���Ƶ���
$hotelIds = unserialize(file_get_contents('hotelIds'));
$userIds  = unserialize(file_get_contents('userIds'));

//��tripadvisor���������ݱ�����־���
//����Ŀ¼������ÿһ���ļ�
$fileNum = 0;
$totalMatrix = array();
$valueMatrix = array();
$roomsMatrix = array();
$locationMatrix = array();
$cleanlinessMatrix = array();
$service1Matrix = array();
$service2Matrix = array();
$businessMatrix = array();
foreach( glob($dirBase.'\*.txt') as $filename)
{
	//���ÿһ���ļ�����ȡ��ÿ�����ָ��Ĵ�־��󣬴洢�ڶ�ά������
	
	$fileContent = file_get_contents($filename);
	//ͨ���ļ������hotelId��
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = $hotelIds[substr($filename,$posStart+1,$posEnd - $posStart-1)]; //�õ�hotelId
	
	//������ʽ����û�������Ȼ����userId��������ʽͬʱ��ô������
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)\n.+\n.+\n<Rating>(.+)\n/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $key=>$user)
	{
		$userId = $userIds[$user];
		$ratingArray = explode('	',$usersArray[2][$key]);
		$totalMatrix[$userId][$hotelId] = $ratingArray[0];
		$valueMatrix[$userId][$hotelId] = $ratingArray[1];
		$roomsMatrix[$userId][$hotelId] = $ratingArray[2];
		$locationMatrix[$userId][$hotelId] = $ratingArray[3];
		$cleanlinessMatrix[$userId][$hotelId] = $ratingArray[4];
		$service1Matrix[$userId][$hotelId] = $ratingArray[5];
		$service2Matrix[$userId][$hotelId] = $ratingArray[6];
		$businessMatrix[$userId][$hotelId] = $ratingArray[7];
	}
	$fileNum++;
}
file_put_contents('totalMatrix',serialize($totalMatrix));
file_put_contents('valueMatrix',serialize($valueMatrix));
file_put_contents('roomsMatrix',serialize($roomsMatrix));
file_put_contents('locationMatrix',serialize($locationMatrix));
file_put_contents('cleanlinessMatrix',serialize($cleanlinessMatrix));
file_put_contents('service1Matrix',serialize($service1Matrix));
file_put_contents('service2Matrix',serialize($service2Matrix));
file_put_contents('businessMatrix',serialize($businessMatrix));
echo $fileNum."\r\n";
?>