#include "windows/miniwindows.h"

#include "qcommon/qcommon.h"

const bool is_dedicated_server = true;

void Sys_InitTime();

void ShowErrorAndAbortImpl( const char * msg, const char * file, int line ) {
	printf( "%s (%s:%d)\n", msg, file, line );
	abort();
}

void Sys_Quit() {
	Qcommon_Shutdown();
	exit( 0 );
}

void Sys_Init() {
	SetConsoleOutputCP( CP_UTF8 );
	Sys_InitTime();
}

int main( int argc, char ** argv ) {
	int64_t oldtime, newtime, time;

	Qcommon_Init( argc, argv );

	oldtime = Sys_Milliseconds();

	while( true ) {
		FrameMark;

		do {
			ZoneScopedN( "Interframe" );

			newtime = Sys_Milliseconds();
			time = newtime - oldtime;
			if( time > 0 ) {
				break;
			}
			Sys_Sleep( 0 );
		} while( 1 );
		oldtime = newtime;

		Qcommon_Frame( time );
	}

	return 0;
}
