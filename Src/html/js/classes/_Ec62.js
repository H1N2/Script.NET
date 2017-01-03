/*******************************************\
  ��Ϸ��ԭ��Ec62�����㷨��(2006-8-3)
  This JavaScript was writen by Dron.
  @2003-2008 Ucren.com All rights reserved.
\*******************************************/
/*
|| ע�⣬Ec62 �㷨�õ� MD5������ DronFw �����
|| �б������ٱ��� MD5 �෽���������У�������ָ
|| ������ MD5 �࣬Ec62 ���Զ����ء�
*/
DronFw.Class.Ec62 = function ()
{
	//62���Ʊ�
	var numbase = "0123456789" +
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
		"abcdefghijklmnopqrstuvwxyz";
	var md5 = new DronFw.Class.MD5();
	function to62scale(n)//��10������ת����62������
	{
		var re = "";
		while(n>=62)
		{
			re = numbase.charAt(n%62) + re;
			n = Math.floor(n/62);
		}
		re = numbase.charAt(n%62) + re;
		re = "000".substr(0,3-re.length) + re;
		return re;
	}
	function to10scale(word)//��62������ת����10������
	{
		var re = 0;
		re = numbase.indexOf(word.charAt(0))*3844;
		re += numbase.indexOf(word.charAt(1))*62;
		re += numbase.indexOf(word.charAt(2));
		return re;
	}
	this.Encode = function(str,password)//���ܺ���
	{
		if (str==""||password=="") return false;
		var re = "";
		for (var i=0; i<str.length; i++) re += to62scale(str.charCodeAt(i) + password.charCodeAt(i%password.length));
		re = md5.calc(password,32).substr(0,6).toLowerCase() + re;
		return re;
	}
	this.Decode = function(str,password)//���ܺ���
	{
		if (str==""||password=="") return false;
		if (md5.calc(password,32).substr(0,6).toLowerCase()!=str.substr(0,6)) return false;
		var re = "",l = password.length;
		for(var i=6; i+3<=str.length; i+=3) re += String.fromCharCode(to10scale(str.substr(i,3))-password.charCodeAt((i/3-2)%l));
		return re;
	}
};
(function ()
{
	// ���� MD5 ��
	document.write("<script type=\"text/javascript\" src=\"" +DronFw.Path+ "classes/_MD5.js\"></script>");
})();