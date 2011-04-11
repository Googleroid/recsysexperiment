<?php

//ѡ��hotel�ǳ����ף���Ϊhotel�Ĵ�����ݶ���һ���ļ��У�ѡ��hotel��ϣ���ѡ��user
// �Ƚ����۸�������һ�������У�Ȼ����������ӡ�������˹�����Ҫ���ٸ�

function saveArrayToFile($array,$filename)
{
	$str = '';
	foreach($array as $key=>$item)
	{
		$str .= $key."\t".$item."\n";
	}
	file_put_contents($filename,$str);
}

$dirBase = '../../tripDataset_o'; 

$hotelCountArray = array();

foreach( glob($dirBase.'/*.txt') as $filename)
{
	$userNum = 0;
	//���ÿһ���ļ�����ȡ���Ƶ��id
	$nameArray = explode('/',$filename);
	$fileSub = $nameArray[count($nameArray)-1];
	$posStart = strpos($fileSub,'_');
	$posEnd = strpos($fileSub,'_',$posStart+1);
	$hotelId = substr($fileSub,$posStart+1,$posEnd - $posStart-1);
	
	//�����ÿһ���û���ţ��ȶ�ȡ�ļ����ݣ�Ȼ����������ʽƥ��,
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)/i',$rateContent,$usersArray);
	$userNum = count($usersArray[1]);
	$hotelCountArray[$hotelId] = $userNum;
}
asort($hotelCountArray);
file_put_contents('tmpData/hotelCountArray',serialize($hotelCountArray));
saveArrayToFile($hotelCountArray,'tmpData/hotelCount');

/*
$hotelCountArray = unserialize(file_get_contents('tmpData/hotelCountArray'));
//�������е�ÿһ���ļ����ҳ�����������Ŀ����10�����û���Ȼ��ɾ�����ǵ�����
$userCountArray =  array();
foreach( glob($dirBase.'\*.txt') as $filename)
{
	$userNum = 0;
	
	//ͨ���ļ������hotelId��
	$posStart = strpos($filename,'_');
	$posEnd = strpos($filename,'_',$posStart+1);
	$hotelId = substr($filename,$posStart+1,$posEnd - $posStart-1);//�õ�hotelId
	
	if($hotelCountArray[$hotelId] < 50)continue; //���������û�����50���ľƵ�
	
	//������ʽ����û�������Ȼ����userId��������ʽͬʱ��ô������
	$rateContent = file_get_contents($filename);
	preg_match_all('/<Author>(.+)\n.+\n.+\n<Rating>(.+)\n/i',$rateContent,$usersArray);
	foreach($usersArray[1] as $key=>$user)
	{
		if(isset($userCountArray[$user])) ++$userCountArray[$user];
		else $userCountArray[$user] = 1;
	}
}

asort($userCountArray);
file_put_contents('tmpData/userCountArray',serialize($userCountArray));
saveArrayToFile($userCountArray,'tmpData/userCount');  //�۲췢�֣�����4�����۵��û�ֻ��1967����ȡ��Щ�û���Ϊʵ�����
//���¹���ʵ�����ݼ��ϣ�����ɸѡ���������¹��졣������������ݼ��Ϸ���select/data�У��ظ��������ϵĺ����������õ�������
//�Ϳ��Է����ڴ�����8M�����ݴ����������ǱȽ����ɵġ�

//����ά����洢���ļ���

*/
?>