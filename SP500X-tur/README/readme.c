/*��Ҫmodbus�Ĵ��� �趨��ȡ��*/
/*

	uint16		configStatus;				 41004	16bit integer	read  ����״̬(0ϵͳδ����,1ϵͳ������)
	
	uint16		firstCalibSolution;			44002	32bit float		r/w  ��һ��궨ֵ 
	uint16		calibCommand;				 44004	16bit integer    r/w �궨���� 1У׼  2(4-20ma����)  3������λ
	float    K;                    44005  �絼��ϵ��
	
  uint16		filterCoefficient_1;		 45002	 32bit float     r/w ���� 4MAֵ 
  uint16		filterCoefficient_2;		 45003	 32bit float     r/w ����20MAֵ 	
	
	float		sensorValue;				 46001	32bit float		read  ����ֵ
	float		sensorValue_mA;				 46003	32bit float		read  ����ֵ(4-20ma��ʽ)
	float   temperatureValue;			 46005	32bit float		read  �¶�(���϶�)

*/	


/*��ʼ״̬˵��*/
/*  
	comm_settings.modbusAddr=0x0E;	�ӻ���ַ14
	comm_settings.modbusDatabits=8;8λ����
	comm_settings.modbusParity=MODBUS_PARITY_	EVEN żУ��
	comm_settings.modbusBaud=9600;9600������
	
	filter_settings.filterCoefficient_2=10000;    //Ĭ�ϲ�����ΧΪ10000
	filter_settings.filterCoefficient_1=0;
	
	calib_settings.firstCalibSolution=0;//��һУ׼ֵ
	calib_settings.calibCommand=8;    ������ѯ�Ĵ���ֵ  8ľ�й��ܣ�ֱ���˳�
	calib_settings.K=0.30;            ��ʼKֵ	
	calib_settings.Ra=510;						�ĵ�λ����
	calib_settings.Rb=1500;
	calib_settings.Rc=6800;
	calib_settings.Rd=27000;	
*/
