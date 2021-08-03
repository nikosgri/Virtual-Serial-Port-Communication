#ifndef __TERMINAL_SETTINGS_H_
#define __TERMINAL_SETTINGS_H_



/**
 *@name  set_block : Blocks reading operation
 *@param fd        : The accept file descriptor 
 *@param block     : Blocks reading or make read()  waits always for bytes 
 *@return		       : Nothing 
 */
void set_block(int fd,int block);
/**
 *@name set_attributes : Setting Terminal attributes , baud rate speed and specifies if the parity bit is up or not.
 *@param  fd		       : The accept file descriptor
 *@param  speed        : in/out speed  baud rate
 *@param  parity       : Enable or disable parity bit
 *@return              : Nothing
 */
void set_attributes(int fd,int speed,int parity);


#endif /* !define __TERMINAL_OPTIONS_H_ */
