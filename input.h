#ifndef _THREAD_INPUT_H_
#define _THREAD_INPUT_H_

void 		i_start_input_thread	();
void 		i_stop_input_thread		();

int			i_v_empty				();
int			i_p_full				();
int			i_tryp_full				();

char	*	i_get_buffer			();

#endif /*_THREAD_INPUT_H_*/
