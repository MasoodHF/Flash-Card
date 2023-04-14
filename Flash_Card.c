/******************************************************************************
 Adding, editing and removing words and their explanations in order to revise
 them automatically It works on Terminal on Linux and Windows OSes. The main
 menu shows the Commands. By adding you can enter a new Word, its Meaning and
 its Example in separate Entries. By deleting Command, the entire buffer will
 be checked for finding the Word.

- After approving deletion, it will remove the entire Filed of that Word. BY
 editing Command you can change them respectively, but by pressing ENTER
 and leave it blank it won't change.
- On the other hand, if you want to remove a Word, its definition or its example
 you have to enter at least one space. It will remove extra spaces following the
 end of string. By revising, it looks for every Word that has been reached to
 the revise time.
- Assume that every time you remember the Definition and the Example of the
 Word showed, the next rives time would be twice longer than the previous one.
- For a new Word, the first revise time is the next day, and the second revise
 time is two days later after the last revision,
 and so on...(1,2,4,8,16,32,64,128,256...)
- After eighth or ninth step, if you passed them respectively, of course, you
 can not only be sure that you never forget that Word, but also you are able to
 use them actively.
 ******************************************************************************//* ==>> PASSED */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "/usr/include/syscall.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
/******************************************************************************
*                                                                             *
*******************************************************************************//* ==>> PASSED */
#define TRUE  1
#define FALSE 0
#define SHORT_BUFFER_SIZE 128
#define LINE_BUFFER_SIZE 1024
#define RECORD_BUFFER_SIZE (4096 + 1)    /* 1 byte for ending character '\0' */
#define MAIN_BUFFER_SIZE (4096 * 1000)    
#define ONE_DAY_IN_SECONDES (60 * 60 * 24)
/******************************************************************************
*  one record for each word                                                   *
*******************************************************************************//* ==>> PASSED */
time_t revise_time                  = 0; /*                               */
time_t creation_time                = 0; /* for saving time for each word */              
time_t previous_time                = 0; /*                               */              
char   word[LINE_BUFFER_SIZE]       = {'\0'}; /* as an active buffer  */
char   example[LINE_BUFFER_SIZE]    = {'\0'}; /* for saving word, its */
char   definition[LINE_BUFFER_SIZE] = {'\0'}; /* definition & example */
/******************************************************************************
*  global variables                                                           *
*******************************************************************************//* ==>> PASSED */
FILE  *file_pointer                          = NULL;                                 
time_t unsigned_long_int_time_t              = 0;               
unsigned long int length_of_main_buffer      = 0;          
unsigned long int main_buffer_active_counter = 0;          
char END_CHARACTER[]            = {'\0'};       
char CRLF_CHARACTER[]           = {'\n'};       
char CRLF_END_CHARACTERS[]      = {'\n', '\0'}; 
char DOTS_CRLF_END_CHARACTERS[] = {'.', '.', '.', '\n', '\0'}; 
	/*----------------------------------------------------------*/
	/*	     strlen(END_CHARACTER)       == 0;              */
	/*           strlen(CRLF_CHARACTER)      == 0;              */
	/*           strlen(CRLF_END_CHARACTERS) == 1;              */
	/*----------------------------------------------------------*/
	/* but strlen(str + END_CHARACTER)       == strlen(str)     */
	/* but strlen(str + CRLF_CHARACTER)      == strlen(str) + 2 */
	/* but strlen(str + CRLF_END_CHARACTERS) == strlen(str) + 1 */
	/*----------------------------------------------------------*/
char file_name[SHORT_BUFFER_SIZE]      = "FC.txt";           
char main_buffer[MAIN_BUFFER_SIZE]     = {'\0'};                     
char line_buffer[LINE_BUFFER_SIZE]     = {'\0'};                     
char short_buffer[SHORT_BUFFER_SIZE]   = {'\0'};
char revers_buffer[SHORT_BUFFER_SIZE]  = {'\0'};
char record_buffer[RECORD_BUFFER_SIZE] = {'\0'};
char extra_main_buffer[MAIN_BUFFER_SIZE + RECORD_BUFFER_SIZE] = {'\0'};
/******************************************************************************
* functions                                                                   *
*******************************************************************************//* ==>> PASSED */
void    add_word(void);               
void    edit_word(void);              
void    list_words(void);              
void    delete_word(void);              
void    revise_words(void);            
void    list_records(void);            
void    state_machine(void);                
char   *revers_string(char *string);      
void    reset_global_variables(void);      
time_t  ascii_to_time_t(char *string); 
void    set_record_ready_to_save(void);
char   *time_t_to_ascii(time_t uli_time); 
char   *remove_spaces_from_end(char *string);
char   *remove_spaces_from_start(char *string);
unsigned long int
	get_elements_of_record(char *pointer_to_element);
void    save(char *buffer_pointr,
	     unsigned long int length_of_buffer_to_save,
	     char *file_name);
char   *read_record(char *input_buffer,
		    unsigned long int starting_point_of_input_buffer,
		    int number_of_elements_to_read);
int     get_line(char *output_buffer,
		 int maximum_characters_to_get,
		 char *two_bytes_ending_format);
/******************************************************************************
 * - Opening files                                                            *
 * - Loading data to main buffer from files respectively                      *
 * - Setting the length of main buffer global variable                        *
 * - Loding state machine                                                     *
 * - Clearing global variables                                                *
 ******************************************************************************//* ==>> PASSED */
int main(int argc, char **argv)
{
	if (argc == 1) {
		if ((file_pointer = fopen(file_name, "a+")) == NULL) {
			printf("Error in creating file %s (%s) (%d)\n",
					   file_name, __FILE__, __LINE__);
			exit(1);
		}
		goto program;
	} else 
		while (--argc > 0)
		{
			if ((file_pointer = fopen(*++argv, "a+")) == NULL)
			{
				printf("Error in creating file %s\nfile (%s)\nline (%d)\n",
					   *argv, __FILE__, __LINE__);
				exit(1);
			}
			strcpy(file_name, *argv);
program:        
			fread(main_buffer, 1, MAIN_BUFFER_SIZE, file_pointer);
			length_of_main_buffer = strlen(main_buffer);
			fclose(file_pointer);
			printf("- Buffer has been loaded! -\n");
			state_machine();
			reset_global_variables();
		}
	return 0;
}
/******************************************************************************
 * - State machine (menu driver)                                              *
 ******************************************************************************//* ==>> PASSED */
void state_machine(void)
{
	for(;;)
	{
		printf( "|-------------------------------|\n"
			"|    You are working on file  ==> %s\n"
			"|                  File size  ==> %lu Bytes\n"
			"|-------------------------------|\n"
			"|  Choose a command!            |\n"
			"|     (a) add new word          |\n"
			"|     (c) clear screen          |\n"
			"|     (d) delete word           |\n"
			"|     (e) edit word             |\n"
			"|     (l) list words            |\n"
			"|     (L) List records          |\n"
			"|     (r) revise today's words  |\n"
			"|     (x) exit                  |\n\n\t",
		 			file_name, length_of_main_buffer);

		scanf("%s", short_buffer);
		switch (short_buffer[0])
		{
			case 'a' : add_word();      break;
			case 'e' : edit_word();     break;
			case 'l' : list_words();    break;
			case 'd' : delete_word();   break;
			case 'L' : list_records();  break; 
			case 'c' : system("clear"); break;
			case 'r' : system("clear");
				   revise_words();  break;
			case 'x' : goto end ;       break;
			default  : system("clear");
				   printf("\nwrong command!\nTry again.\n");
						    break;
		}
	}
	end:
}
/******************************************************************************
 * - Adding a new word to the buffer and save it on the storage               *
 * - It contains six items in two groups:                                     *
 *    (3 automatic generated integers as time and 3 strings as input)         *
 *   line number   :   functions                                              *
 *  ----------------------------------------                                  *
 *   		   : getchar(void)      	                              *
 *                 : time_to_ascii(time_t)                                    *
 *                 : strcat(cahr *, char *)                                   *
 *                 : get_line(char *, int, char *)                            *
 *                 : strlen(char *)                                           *
 *                 : set_record_ready_to_save()                               *
 *                 : save(char *, unsigned long int, char *)                  *
 ******************************************************************************//* ==>> PASSED */
void add_word(void)
{
	getchar();
	printf("enter a word:\n");
	get_line(word, LINE_BUFFER_SIZE, "wCRLFwEND");
	if (strlen(word) == 1)
                goto end;
	else
	printf("enter the definition:\n");
	get_line(definition, LINE_BUFFER_SIZE, "wCRLFwEND");
	printf("enter an example:\n");
	get_line(example, LINE_BUFFER_SIZE, "wCRLFwEND");
	set_record_ready_to_save();
	if (strlen(main_buffer) + strlen(record_buffer) < MAIN_BUFFER_SIZE)
	{
		strcat(main_buffer, record_buffer);
		save(main_buffer, strlen(main_buffer), file_name);
	}
	else
	{
		printf( "Error! Buffer is full.\n Adding new record is not possible.\n"
			"But you can make a new file inorder to save the new word.\n");
question_point:
		printf("Would you like to enter the name of the new file ? (y/n)");
		scanf("%s", &short_buffer[0]);
		getchar();
		if (short_buffer[0] == 'y' || short_buffer[0] == 'Y')
		{
			printf("Enter the name of the new file: ");
			get_line(short_buffer, SHORT_BUFFER_SIZE, "nCRLFwEND");
			save(record_buffer, strlen(record_buffer), short_buffer);
			printf( "It has been saved. Next time for using it,\n"
				"you can type the name of this new file\n"
				"after program name in terminal and press ENTER!\n");
		}
		else if (short_buffer[0] == 'n' || short_buffer[0] == 'N')
		{
			; /* cancel adding new word */
		}
		else
		{
			printf("Wrong answer! Try again.\n");
			goto question_point;
		}
	}
	end:
	word[0]          = '\0'; /* resetting short term buffers */
	example[0]       = '\0';
	definition[0]    = '\0';
	short_buffer[0]  = '\0';
	record_buffer[0] = '\0';
}
/******************************************************************************
 ******************************************************************************//* ==>> PASSED */
void delete_word(void)
{
	int i = 0;
	short_buffer[0] = '\0';
	char str[LINE_BUFFER_SIZE] = {'\0'};
	unsigned long int length_of_record = 0l;
	printf("enter the word you want to delete:\n"); 
	getchar();
	get_line(str, LINE_BUFFER_SIZE, "wCRLFwEND");
	main_buffer_active_counter = 0;
	while (main_buffer_active_counter < length_of_main_buffer)
	{
		length_of_record =
		get_elements_of_record(read_record(main_buffer,
						   main_buffer_active_counter, 6));
		main_buffer_active_counter += length_of_record;
		if (strcmp(word, str) == 0)
		{ /* found */
			printf("*** One word has been found:\n");
			printf("*************************************\n");
			printf("***       word:> %s", word);
			printf("*** definition:> %s", definition);  
			printf("***    example:> %s", example);
			printf("*************************************\n");
			question_point:                              /* Authentication */
			printf("Do you really want to delete this record? (y/n) ");
			scanf("%s", short_buffer);
			if (short_buffer[0] == 'y' || short_buffer[0] == 'Y')
			{
				if (main_buffer_active_counter >= length_of_main_buffer)
				{
					main_buffer[main_buffer_active_counter - length_of_record] = '\0';
				}
				else
				{
					i = 0;
					while (i <= (length_of_main_buffer - main_buffer_active_counter))
					{
						main_buffer[main_buffer_active_counter - length_of_record + i] =
								main_buffer[main_buffer_active_counter + i]; 
						i++;
					}
					main_buffer[length_of_main_buffer - length_of_record] = '\0';
				}
				length_of_main_buffer = strlen(main_buffer);
				main_buffer_active_counter -= length_of_record;
				save(main_buffer, strlen(main_buffer), file_name);
			}
			else if (short_buffer[0] == 'n' || short_buffer[0] == 'N')
			{
				goto next; /* cansel deliting */
			}
			else
			{
				printf("Wrong answer!\n");
				goto question_point;
			}
		} 
		next:
	}
}
/******************************************************************************
 * - Editing a word, its definition & esamples and saving it on the storage   *
 * - By leaving each field blank and pressing ENTER, that element will not be *
 *   change.                                                                  *
 * - Used functions: getchar(void)                                            *
 *                 : time_to_ascii(time_t)                                    *
 *                 : strcat(cahr *, char *)                                   *
 *                 : get_line(char *, int, char *)                            *
 *                 : strlen(char *)                                           *
 *                 : printf(...)                                              *
 *                 : save(char *, unsigned long int, char *)                  *
 ******************************************************************************//* ==>> PASSED */
void edit_word(void)
{
	int i = 0;
	char str[LINE_BUFFER_SIZE];
	char search_str[LINE_BUFFER_SIZE];
	int record_has_been_changed = FALSE;
	unsigned long int length_of_old_record = 0;
	record_buffer[0] = '\0';
	extra_main_buffer[0] = '\0';
	printf("enter the word you want to edit:\n"); 
	getchar();
	get_line(search_str, LINE_BUFFER_SIZE, "wCRLFwEND");
	main_buffer_active_counter = 0;
	while (main_buffer_active_counter < strlen(main_buffer))
	{
		length_of_old_record =
		get_elements_of_record(read_record(main_buffer,
						   main_buffer_active_counter, 6));
		main_buffer_active_counter += length_of_old_record;
		if (strcmp(word, search_str) == 0)
		{ /* found */
			printf("*** One word has been found:\n");
			printf("*** word:       %s", word);
			printf("*** definition: %s", definition);
			printf("*** example:    %s", example);
			printf("Edit the word:\n");
			get_line(str, LINE_BUFFER_SIZE, "wCRLFwEND");
			if (strlen(str) != 1)
			{
				strcpy(word, str);
				record_has_been_changed = TRUE;
			}
			printf("Edit the definition:\n");
			get_line(str, LINE_BUFFER_SIZE, "wCRLFwEND");
			if (strlen(str) != 1)
			{
				strcpy(definition, str);
				record_has_been_changed = TRUE;
			}
			printf("Edit the example:\n");
			get_line(str, LINE_BUFFER_SIZE, "wCRLFwEND");
			if (strlen(str) != 1)
			{
				strcpy(example, str);
				record_has_been_changed = TRUE;
			}
			str[0] = '\0';
			if (record_has_been_changed == FALSE)
			{
				strcat(extra_main_buffer, record_buffer);
				goto next;
			}
			set_record_ready_to_save();
			if (main_buffer_active_counter >= strlen(main_buffer))
			{
				main_buffer[main_buffer_active_counter - length_of_old_record] = '\0';
				search_str[0] = '\0';
			} else {
				i = 0;
				while (i <= (length_of_main_buffer - main_buffer_active_counter))
				{
					main_buffer[main_buffer_active_counter - length_of_old_record + i]
					= main_buffer[main_buffer_active_counter + i];
					i++;
				}
				main_buffer[length_of_main_buffer - length_of_old_record] = '\0';
			}
			length_of_main_buffer = strlen(main_buffer);
			main_buffer_active_counter -= length_of_old_record;
			strcat(extra_main_buffer, record_buffer);
		} else 
			strcat(extra_main_buffer, record_buffer);
		next:
	}
	save(extra_main_buffer, strlen(extra_main_buffer), file_name);
	strcpy(main_buffer, extra_main_buffer);
	length_of_main_buffer = strlen(main_buffer);
	extra_main_buffer[0] = '\0';
}
/******************************************************************************
 ******************************************************************************//* under cunstraction */
void revise_words(void)
{
	int i = 0;
	time_t today_time_t = time(NULL);
	char str[LINE_BUFFER_SIZE] = {'\0'};
	int record_has_been_changed = FALSE;
	unsigned long int length_of_old_record = 0;
	record_buffer[0] = '\0';
	main_buffer_active_counter = 0;
	while (main_buffer_active_counter < strlen(main_buffer))
	{
		length_of_old_record =
		get_elements_of_record(read_record(main_buffer, main_buffer_active_counter, 6));
		main_buffer_active_counter += length_of_old_record;
		if (revise_time < today_time_t)
		{ /* found */
			system("clear");
			printf("+-----------------------------------+\n");
			printf("| Do you know the definition        |\n");
			printf("|    and example of the word? (y/n) |\n");
			printf("+-----------------------------------+\n");
			printf("\n");
			printf("	%s", word);
			printf("\n\n\n\n\n\n\n");
			printf("-----------------------------------------\n");
			printf("| definition |\n");
			printf("+------------+\n  %s", definition);
			printf("\n\n-----------------------------------------\n");
			printf("|  example   |\n");
			printf("+------------+\n  %s", example);
			printf("\n-----------------------------------------\n");
			question_point:
			scanf("%s", &short_buffer[0]);
			getchar();
			if (short_buffer[0] == 'y' || short_buffer[0] == 'Y')
			{
				revise_time = (revise_time - previous_time) * 2 + today_time_t;
			}
			else if (short_buffer[0] == 'n' || short_buffer[0] == 'N')
			{
				revise_time = today_time_t + ONE_DAY_IN_SECONDES;
				previous_time = today_time_t;
			}
			else
			{
				printf("Wrong answer! Try again.\n");
				goto question_point;
			}
			set_record_ready_to_save();
			if (main_buffer_active_counter >= length_of_main_buffer)
			{
				main_buffer[main_buffer_active_counter - length_of_old_record] = '\0';
			}
			else
			{
				i = 0;
				while (i <= (length_of_main_buffer - main_buffer_active_counter))
				{
					main_buffer[main_buffer_active_counter - length_of_old_record + i]
					= main_buffer[main_buffer_active_counter + i];
					i++;
				}
				main_buffer[length_of_main_buffer - length_of_old_record] = '\0';
			}
			length_of_main_buffer = strlen(main_buffer);
			main_buffer_active_counter -= length_of_old_record;
			strcat(main_buffer, record_buffer);
			save(main_buffer, strlen(main_buffer), file_name);
		}
	}

}
/******************************************************************************
 * - Loading one record into the global record_buffer[] buffer and sending    *
 *   back a pointer to that                                                   *
 ******************************************************************************//* ==>> PASSED */
char *read_record(char *input_buffer,
		  unsigned long int starting_point_of_input_buffer,
		  int number_of_elements_to_read)
{
	int i;
	line_buffer[0]   = '\0';
	record_buffer[0] = '\0';
	for (i = 1; i <= number_of_elements_to_read; i++)
	{
		sscanf(input_buffer + starting_point_of_input_buffer
			+ strlen(record_buffer) , "%[^\n]", line_buffer);
		strcat(line_buffer, CRLF_END_CHARACTERS);
		strcat(record_buffer, line_buffer);
	}
	return record_buffer;
}
/******************************************************************************
 * - Caching elements of a record, putting them into the global variables and *
 *   returns the length of the record                                         *
 ******************************************************************************//* ==>> PASSED */
unsigned long int get_elements_of_record(char *pointer_to_element)
{                                                                
	word[0]       = '\0';
	example[0]    = '\0';
	definition[0] = '\0';
	sscanf(pointer_to_element, "%lu\n%lu\n%lu\n%[^\n]\n%[^\n]\n%[^\n]\n",
	  &creation_time, &previous_time, &revise_time, word, definition, example);
	strcat(word, CRLF_END_CHARACTERS);
	strcat(example, CRLF_END_CHARACTERS);
	strcat(definition, CRLF_END_CHARACTERS);
	return strlen(pointer_to_element);
}
/******************************************************************************
 * Global usage: buf, main_buffer_active_counter, str, unit,                  *
 *                                                                            *
 *                                                                            *
 * Loading three unsigned long int, three strings                             *
 ******************************************************************************//* under cunstraction */
void find_word(char *string)
{
	unsigned long int length;
	while ((length = get_elements_of_record(read_record(main_buffer,
								0, 6))) > 0) ;   
}
/******************************************************************************
 * Global usage: buf, main_buffer_active_counter, str, unit,                  *
 * Loading three unsigned long int, three strings                             *
 ******************************************************************************//* ==>> PASSED */
void list_words(void) 
{
	int i;
	int count = 0;
	main_buffer_active_counter = 0;
	while (main_buffer_active_counter < length_of_main_buffer)
	{
		main_buffer_active_counter +=
			get_elements_of_record(read_record(main_buffer,
						main_buffer_active_counter, 6));
		printf("%4d - %s", count + 1, word);
		count++;
	}
	printf("----------- End of %d words list.\n", count);
}
/******************************************************************************
 * Global usage: buf, main_buffer_active_counter, str, unit,                  *
 *                                                                            *
 ******************************************************************************//* ==>> PASSED */
void list_records(void) 
{
	int count = 0;
	char *pointer;
	main_buffer_active_counter = 0;
	while (main_buffer_active_counter < length_of_main_buffer)
	{
		pointer = read_record(main_buffer, main_buffer_active_counter, 6);
		main_buffer_active_counter += strlen(pointer); 
		printf("%s\n", pointer);
		count++;
	}
	printf("--------- End of %d records list.\n", count);
}
/******************************************************************************
 *  Using globale variables and recor buffer                                  *
 ******************************************************************************//* ==>> PASSED */
void set_record_ready_to_save(void)
{
	record_buffer[0] = '\0';
	unsigned_long_int_time_t = time(NULL);
	creation_time = unsigned_long_int_time_t;
	previous_time = unsigned_long_int_time_t;
	revise_time   = unsigned_long_int_time_t + ONE_DAY_IN_SECONDES;
	strcat(record_buffer, time_t_to_ascii(creation_time));
	strcat(record_buffer, CRLF_END_CHARACTERS);
	strcat(record_buffer, time_t_to_ascii(previous_time));
	strcat(record_buffer, CRLF_END_CHARACTERS);
	strcat(record_buffer, time_t_to_ascii(revise_time));
	strcat(record_buffer, CRLF_END_CHARACTERS);
	remove_spaces_from_end(word);
	remove_spaces_from_start(word);
	if (strlen(word) == 1)
	{
		word[0] = '\0';
		strcat(word, DOTS_CRLF_END_CHARACTERS);
	}
	strcat(record_buffer, word);
	remove_spaces_from_end(definition);
	remove_spaces_from_start(definition);
	if (strlen(definition) == 1)
	{
		definition[0] = '\0';
		strcat(definition, DOTS_CRLF_END_CHARACTERS);
	}
	strcat(record_buffer, definition);
	remove_spaces_from_end(example);
	remove_spaces_from_start(example);
	if (strlen(example) == 1)
	{
		example[0] = '\0';
		strcat(example, DOTS_CRLF_END_CHARACTERS);
	}
	strcat(record_buffer, example);
}
/******************************************************************************
 ******************************************************************************//* ==>> PASSED */
char *remove_spaces_from_end(char *string)
{
	long int i;
	int counter = 0;
	i = strlen(string) - 1;
	if (i != 0)
	{
		if (string[i] == '\n')
			i--;
		while (string[i] == ' ')
		{
			i--;
			counter++;
		}
		if (counter == strlen(string) - 1)
			strcpy(string, DOTS_CRLF_END_CHARACTERS);
		else
		{
			string[i + 1] = '\n';
			string[i + 2] = '\0';
		}
	}
	return string;
}
/******************************************************************************
 ******************************************************************************//* ==>> PASSED */
char   *remove_spaces_from_start(char *string)
{
	long int i;
	int count = 0;
	int counter = 0;
	i = strlen(string) - 1;
	if (string[counter] == ' ' && counter <= i)
		counter++;
	while (counter <= i)
	{
		string[count] = string[counter];
		count++;
		counter++;
	}
	string[count] = '\0';
	return string;
}
/******************************************************************************
 * - Converting ascii to integer                                              *
 * - unsigned long int : returning an integer                                 *
 * - char *str : pointer to an incomming string                               *
 ******************************************************************************//* ==>> PASSED */
time_t ascii_to_time_t(char *string)
{
	int counter = 0;
	time_t sum = 0L;
	int in;
	while ((in = string[counter]) >= '0' && in <= '9')
	{
		sum = sum * 10 + (in - '0');
		counter++;
	}
	return sum;
}
/******************************************************************************
 * - Converting integer to ascii                                              *
 * - char * : returns a pointer to string                                     *
 * - unsigned long int l_time : input number mostly from time(NULL)           *
 ******************************************************************************//* ==>> PASSED */
char *time_t_to_ascii(time_t uli_time)
{
	int reminder;
	int counter = 0;
	revers_buffer[0] = '\0';
	while (uli_time > 0)
	{
		reminder = uli_time % 10;
		uli_time /= 10;
		revers_buffer[counter] = reminder + '0';
		counter++;
	}
	revers_buffer[counter] = '\0';
	return revers_string(revers_buffer);
}
/******************************************************************************
 * - Reverses a string                                                        *
 * - char * : pointer to the returning string                                 *
 * - char *str : pointer to the recieved string                               *
 ******************************************************************************//* ==>> PASSED */
char *revers_string(char *string)
{
	char temp;
	int counter = 0;
	int n = strlen(string) - 1;
	while (counter < n)
	{
		temp = string[n];
		string[n] = string[counter];
		string[counter] = temp;
		n--;
		counter++;
	}
	return string;
}
/******************************************************************************
 * - Saves buffer to storage                                                  *
 * - char *buffer : pointer to a character array string for reading data      *
 * - char *file_name : pointer to a string name of file for saveing data      *
 * - unsigned long int : length of buffer for writing                         *
 ******************************************************************************//* ==>> PASSED */
void save(char *buffer_pointer,
	  unsigned long int length_of_buffer_to_save,
	  char *file_name)
{
	if ((file_pointer = fopen(file_name, "w+")) == NULL)
	{
		printf("Error in creating file %s (%s) (%d)\n",
					file_name, __FILE__, __LINE__);
		exit(1);
	}
	fwrite(buffer_pointer, 1, length_of_buffer_to_save, file_pointer);
	fclose(file_pointer);
	length_of_main_buffer = strlen(main_buffer);
}
/******************************************************************************
 * - Getting string fron standard input and put it in the buffer and return   *
 *   an integer as the length of the string.                                  *
 * - int ()    : returns integer for number of characters received            *
 * - char *buffer_to_save_line: character pointer for saving them             *
 * - int max   : maximum number of input characters                           *
 * - char *end_2_bytes : If adding CRLF and END character at the end of the   *
 *   string						  	              *
 *               "nCRLFwEND"  = No   '\n' With '\0'                           *
 *               "nCRLFnEND"  = No   '\n' No   '\0'		              *
 *               "wCRLFwEND"  = With '\n' With '\0'                           *
 *               "wCRLFnEND"  = With '\n' No   '\0'                           *
 ******************************************************************************//* ==>> PASSED */
int get_line(char *buffer_to_save_line,
	     int maximum_characters_to_get,
	     char *two_bytes_ending_format)
{
	int i;
	char c;
	for (i = 0; i < (maximum_characters_to_get - 1) &&
				(c = getchar()) != EOF && c != '\n'; ++i)
		*(buffer_to_save_line + i) = c;
	if (c == '\n')
	{
		if (strcmp(two_bytes_ending_format, "wCRLFwEND") == 0)
		{
			    *(buffer_to_save_line + i) = c;
			    ++i;
		}
		else if (strcmp(two_bytes_ending_format, "nCRLFwEND") == 0)
		{
			;
		}
		else if (strcmp(two_bytes_ending_format, "nCRLFnEND") == 0)
		{
			i--;
			goto end;
		}
		else if (strcmp(two_bytes_ending_format, "wCRLFnEND") == 0)
		{
			*(buffer_to_save_line + i) = c;
			goto end;
		}
		else
		{
			printf("Wrong format for get_line() in file (%s) line (%d)\n",
					__FILE__, __LINE__);
			exit(1);
		} 
	} 
	*(buffer_to_save_line + i) = '\0';
	end:
	return i;
}
/******************************************************************************
 * - Clearing global variables                                                *
 ******************************************************************************//* ==>> PASSED */
void reset_global_variables(void)
{
	word[0]                    = '\0'; 
	example[0]                 = '\0'; 
	definition[0]              = '\0'; 
	main_buffer[0]             = '\0';           
	record_buffer[0]           = '\0';
	extra_main_buffer[0]       = '\0';
	file_pointer               = NULL;                       
	revise_time                = 0;		
	creation_time              = 0;     
	previous_time              = 0;      
	length_of_main_buffer      = 0;  
	unsigned_long_int_time_t   = 0;
	main_buffer_active_counter = 0; 
}
