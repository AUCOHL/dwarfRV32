
void *memcpy(void *aa, const void *bb, long n)
{
	char *a = aa;
	const char *b = bb;
	while (n--) *(a++) = *(b++);
	return aa;
}
