#include <linux/sched.h> //for task_struct and TODO_struct
#include <linux/slab.h> //kmalloc
#include <asm/uaccess.h> //copy_from_user

static task_t* get_process_if_valid(pid_t pid);

asmlinkage long sys_add_TODO (pid_t pid, const char* TODO_description, ssize_t description_size)
{
	if (!TODO_description || description_size < 1) {
		printk ("sys_add_TODO error: returning EINVAL\n");
		return EINVAL;
	}
	task_t *process;
	process = get_process_if_valid(pid);
	if (!process) {
		printk ("sys_add_TODO error: returning ESRCH\n");
		return ESRCH;
	}
	struct TODO_struct *new_TODO = kmalloc(sizeof(struct TODO_struct), GFP_KERNEL);
	if (!new_TODO) {
		printk ("sys_add_TODO error: returning ENOMEM\n");
		return ENOMEM;
	}
	new_TODO->description_size = description_size;
	new_TODO->status = 0;
	new_TODO->TODO_description = kmalloc(sizeof(char)*description_size, GFP_KERNEL);
	if (!new_TODO->TODO_description) {
		kfree(new_TODO);
		printk ("sys_add_TODO error: returning ENOMEM\n");
		return ENOMEM;
	}
	if (copy_from_user(new_TODO->TODO_description, TODO_description, description_size)) {
		kfree(new_TODO->TODO_description);
		kfree(new_TODO);
		printk ("sys_add_TODO error: returning EFAULT\n");
		return EFAULT;
	}
	/* Everything went OK - add the new description to the process' TODO list */
	list_add_tail(&(new_TODO->list),&(process->TODO_queue.list));
	return 0;
}

asmlinkage long sys_read_TODO (pid_t pid, int TODO_index, char* TODO_description, ssize_t description_size, int* status)
{
	//TODO_index < 1 and status !=null didn't appear - verify
	if (!TODO_description || TODO_index < 1 || !status) {
		printk ("sys_read_TODO error: returning EINVAL\n");
		return -EINVAL;
	}
	task_t *process;
	process = get_process_if_valid(pid);
	if (!process) {
		printk ("sys_read_TODO error: returning ESRCH\n");
		return -ESRCH;
	}
	int i=1;
	struct TODO_struct *TODO_entry;
	struct list_head *pos;
	list_for_each(pos, &(process->TODO_queue.list)){
		if (i == TODO_index) {
			TODO_entry = list_entry(pos, struct TODO_struct, list);
			int actual_size;
			if (description_size >= TODO_entry->description_size) {
				actual_size = TODO_entry->description_size;
			}
			else {
				printk ("sys_read_TODO error: returning EINVAL\n");
				return -EINVAL;	
			}
			if (copy_to_user(TODO_description, TODO_entry->TODO_description, actual_size) ||
			    copy_to_user(status, &(TODO_entry->status), sizeof(int))) {
				printk ("status is %d\n", TODO_entry->status);
				printk ("sys_read_TODO error: returning EFAULT\n");
				return -EFAULT;
			}
			/* description and size were successfully copied */
			return actual_size;
		}
		i++;	
	}
	/* if we reached here, the TODO_index didn't appear in the queue */
	printk ("sys_read_TODO error: returning EINVAL\n");
	return -EINVAL;
}

asmlinkage long sys_mark_TODO (pid_t pid, int TODO_index, int status)
{
	if (TODO_index < 1) {
		printk ("sys_mark_TODO error: returning EINVAL\n");
		return EINVAL;
	}
	task_t *process;
	process = get_process_if_valid(pid);
	if (!process) {
		printk ("sys_mark_TODO error: returning ESRCH\n");
		return ESRCH;
	}
	int i=1;
	struct TODO_struct *TODO_entry;
	struct list_head *pos;
	list_for_each(pos, &(process->TODO_queue.list)){
		if (i == TODO_index) {
			TODO_entry = list_entry(pos, struct TODO_struct, list);
                	TODO_entry->status = status;
			return 0;
		}
		i++;
	}
	/* if we reached here, the TODO_index didn't appear in the queue */
	printk ("sys_mark_TODO error: returning EINVAL\n");
	return EINVAL;

}
	
asmlinkage long sys_delete_TODO (pid_t pid, int TODO_index)
{
	if (TODO_index < 1) {
		printk ("sys_read_TODO error: returning EINVAL\n");
		return EINVAL;
	}

	task_t *process;
	process = get_process_if_valid(pid);
	if (!process) {
		printk ("sys_add_TODO error: returning ESRCH\n");
		return ESRCH;
	}

	int i=1;
	struct TODO_struct *tmp;
	struct list_head *pos,*q;
	list_for_each_safe(pos,q, &(process->TODO_queue.list)){
		if (i == TODO_index) {
                	tmp = list_entry(pos,struct TODO_struct, list);
                	list_del(pos);
                	kfree(tmp->TODO_description);
                	kfree(tmp);
			return 0;
		}
		i++;
	}
	/* if we reached here, the TODO_index didn't appear in the queue */
	printk ("sys_read_TODO error: returning EINVAL\n");
	return EINVAL;
}
	
/* auxilary function used by the TODO system calls.
 * return value: if the requested TODO queue belongs to the calling process
 * or its descendants, return a pointer to that process. else return NULL
 * inputs: pid - the pid of the process with the requested TODO queue
 */ 
static task_t* get_process_if_valid(pid_t pid)
{
	task_t *p, *tmp;
	p = find_task_by_pid(pid);
	if (!p) {
		printk ("get_process_if_valid error: process doesn't exist\n");
		return NULL;
	}
	tmp = p;
	/* Go up the process tree to check whether the requested process is the 
	 * calling process or its descendands. If we reach pid=0 (sched process)
	 * then it's not. */
	while (tmp->pid && tmp->pid != current->pid) tmp = tmp->p_opptr;
	if (!tmp->pid) {
		printk ("get_process_if_valid error: process exists but is not a descendant\n");
		return NULL;
	}
	return p;
}
