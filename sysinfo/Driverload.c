////////////////////////////////////////////////////////
//  pcidebug.dll
//                        Aug 20 1999 kashiwano masahiro
//
////////////////////////////////////////////////////////

#include <windows.h>
/*
�f�o�C�X�h���C�o�h���C�o�[�̓o�^�A�J�n������
�߂�l
	TRUE	����I��
	FALSE	�h���C�o�[�o�^�A�J�n���s
			�f�o�C�X�h���C�o�𐧌�ł��錠�����Ȃ��Ǝ��s����B

����
	filename	�h���C�o�[�̃t�@�C����
	drivername	�h���C�o�[�̖��O�B�h���C�o�[�����ł��閼�O�B
				UnloadDriver�̈����ɂ��g��

*/
BOOL LoadDriver(char *filename, char *drivername)
{
    SC_HANDLE	hSCManager;
    SC_HANDLE	hService;
    SERVICE_STATUS	serviceStatus;
    BOOL	ret;

	// �T�[�r�X�R���g���[���}�l�[�W�����J��
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!hSCManager)
		return FALSE;

	// ���Ƀh���C�o�[�����݂��邩�m�F���邽�߂Ƀh���C�o�[���J��
	hService = OpenService( hSCManager,
							drivername,
							SERVICE_ALL_ACCESS);
	if(hService){
		// ���ɓ��삵�Ă���ꍇ�͒�~�����č폜����
		// �ʏ�̓h���C�o�[�����݂���Ƃ���LoadDriver���Ăяo���Ȃ��̂ŕ��i�͂��肦�Ȃ�
		// �h���C�o�ُ̈킪�l������
		ret = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		ret = DeleteService(hService);
		CloseServiceHandle(hService);
	}
	// �h���C�o�[��o�^����
	hService = CreateService(hSCManager,
		drivername,
		drivername,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER, // �J�[�l���h���C�o
		SERVICE_DEMAND_START,  // ���StartService()�ɂ���ĊJ�n����
		SERVICE_ERROR_NORMAL,
		filename,            // �h���C�o�[�t�@�C���̃p�X
		NULL,NULL,NULL,NULL,NULL);

	ret = FALSE;
    if(hService) {

		// �h���C�o�[���J�n����
		ret = StartService(hService, 0, NULL);

		// �n���h�������
		CloseServiceHandle(hService);
	}
	// �T�[�r�X�R���g���[���}�l�[�W�������
	CloseServiceHandle(hSCManager);

	return ret;
}

/*
�h���C�o�[���~����
�߂�l
	TRUE	����I��
	FALSE	���s

����
	drivername	�h���C�o�[�̖��O�B
*/
BOOL UnloadDriver(char *drivername)
{
	SC_HANDLE	hSCManager;
	SC_HANDLE	hService;
	SERVICE_STATUS  serviceStatus;
	BOOL	ret;

	// �T�[�r�X�R���g���[���}�l�[�W�����J��
    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!hSCManager)
		return FALSE;

    // �h���C�o�[�̃T�[�r�X���J��
    hService = OpenService(hSCManager, drivername, SERVICE_ALL_ACCESS);
	ret = FALSE;
	if(hService) {
	    // �h���C�o�[���~������ 
	    ret = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);

	    // �h���C�o�[�̓o�^������
	    if(ret == TRUE)
	        ret = DeleteService(hService);

	    // �n���h�������
	    CloseServiceHandle(hService);
	}
	// �T�[�r�X�R���g���[���}�l�[�W�������
    CloseServiceHandle(hSCManager);

    return ret;
}
