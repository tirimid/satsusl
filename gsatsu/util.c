// SPDX-License-Identifier: BSD-3-Clause

static u64 u_tickstart;

void
err(char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[O_MAXLOGLEN];
	vsnprintf(msg, O_MAXLOGLEN, fmt, args);
	
	// stderr is backup so that error can still be processed in case a messagebox
	// can't be opened.
	if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, O_ERRWNDTITLE, msg, NULL))
	{
		fprintf(stderr, "err: %s\n", msg);
	}
	
	va_end(args);
}

u64
unixus(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (u64)tv.tv_sec * 1000000 + tv.tv_usec;
}

void
begintick(void)
{
	u_tickstart = unixus();
}

void
endtick(void)
{
	u64 tickend = unixus();
	i64 timeleft = O_TICKUS - tickend + u_tickstart;
	timeleft *= (timeleft > 0);
	
	struct timespec ts =
	{
		.tv_nsec = 1000 * timeleft
	};
	nanosleep(&ts, NULL);
}
