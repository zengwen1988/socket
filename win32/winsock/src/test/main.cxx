#include <xwin/xsock.hxx>
#include <xwin/XSockServerHelper.hxx>

#include <stdio.h>

int main (void)
{

	int ret = XSockServerHelper::startServer("0.0.0.0", 12345);

	printf("ret: %d", ret);

	Sleep(uint32_t(100 * 1e3));

	return 0;

}