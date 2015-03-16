
void mm_new_pgtable(mm_t *mm)
{
	arch_mm_new_pgtable(&(mm->arch_mm));
}
