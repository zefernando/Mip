# Mip
Introdução
O xfer é um simples programa para recepção de arquivos do MIP (Mastercard Interface Processor). O programa opera de duas formas: recepção de “bulk files” e modo advisement. Este último é um simples relatório de status dos arquivos presentes no MIP para recepção ou transferência. Veja mais adiante alguns exemplos. O processo é ativado via linha de comando e controlado por um arquivo de configuração. Parâmetros definidos na linha de comando tem prioridade sobre os mesmos parâmetros definidos no arquivo de configuração
O MIP opera em EBCDIC, e os dados recebidos podem ser convertidos para ASCII com o uso da opção “a”. No caso do advisement, a conversão é automática, naturalmente. A listagem de advisement, bem como toda informação gerada on screen, é emitida em stderr
Durante a transferência, um relatório de status pode ser emitido a cada “n” blocos, ou simplesmente teclando “enter”
Um arquivo de log registra informações de contrôle e pode ser configurado para gravar um dump de cada mensagem enviada ou recebida do MIP. Um parâmetro opcional indica um limite para o tamanho deste arquivo, atingido o qual um novo é gerado
Os arquivos presentes no MIP tem um tipo, no formato Tnnn ou Rnnn, T para arquivos orginados da Mastercard e R para arquivos com destino à Mastercard. O valor “nnn” é o bulk type. Gerações destes arquivos são diferenciadas pela data juliana jjj, com 3 dígitos, e número de sequência ss, com 2 dígitos. Não sendo especificado o nome do arquivo a ser gerado no host, o arquivo gerado será exatamente Tnnnjjjss
Características
	Uso via linha de comando
	Programa único para recepção e advisement
	Arquivo de parâmetros com possibilidade de redefinição de valôres na linha de comando
	Estatísticas de transmissão/advisement gravadas no log do sistema
	Emissão automática ou manual de relatório de status
	Salvamento automático do arquivo de log a partir de um tamanho configurável
 
Sintaxe (v 1.0.0.1)
xfer –t bulk_file [[ j Julian_day] [ s SeqNo]] [ r] [ a] [ d] [LocalFile]
Opções na linha de comando
-t Bulk_Type
formato “ Tnnn”  ou “A” ou “AC”. Para advisement, “C” indica o status desejado. Ex: -tAS lista os bulk types com status “staged”
 j Julian Day
Formato nnn. Campo numérico com a data juliana do arquivo
 s SeqNo
Fornece o número de sequencia. Só é considerado quando for fornecida a data juliana
 d
Ativa gravação no log de todas as mensagens recebidas e transmitidas, em hexadecimal e em modo texto, para trace
-a
Define conversão para ASCII do arquivo de saída
-r
Permite apagar o arquivo de saída se este já existir.
LocalFile
Nome do arquivo a ser gerado. Se não for informado na linha de comando ou no arquivo de configuração, será gerado arquivo com o nome TNNNJJJSS, para o caso de recepção de Bulk Files. NNN, JJJ e SS correspondem aos valores informados pelo MIP para Bulk Type, data juliana e número de sequência. Para o advisement será gerado arquivo “MIP.txt”
Exemplos:
xfer –t T008 –d
Recebe o bulk file type T008, gera informações de trace
xfer –tA advise
Recebe o status de todos os arquivos no MIP, grava em “advise”
xfer –tAS
Recebe o status de todos os arquivos no MIP com status S, grava em “MIP.txt”
 xfer –r –a –t T701 –d 237 –s 01
Recebe o arquivo 701 com data de 237 (25 de agosto, para 1999), sequência 01, e grava em “T7012371”, mesmo que o arquivo já exista. Antes de gravar, converte para ASCII
Parâmetros para o arquivo de configuração
o arquivo xfer.cfg tem parâmetros para a configuração da conexão X25 e da recepção ou advisement, na forma usual de pares variável=valor. Segue a lista dos valores tratados, em ordem alfabética. Veja exemplos mais adiante

Bulk_File=
Formato Tnnn ou Ac. Deve estar no formato Tnnn para recepção ou AC para o advisement.
ConvertToASCII=
Indica se o arquivo de saída vai ser convertido. Padrão: não converte
Debug_Level=
Modo teste, se “Y” gera grande quantia de informações para debug
Endpoint=
endereço do MIP para a MasterCard. Padrão: 01940
ICA=
ICA number para a sessão. Padrão: 7097
Julian_Date=
padrão: data de hoje. Para a data juliana corrente use “date +%j” no FTX
Logical_Channel=
Canal lógico da conexão com o MIP
LogFileName=
Nome do arquivo de log
MaxLogSize=
Tamanho máximo em bytes do arquivo de log. Ao atingir este tamanho um novo arquivo é gerado. O anterior tem a extensão “.bak” acrescentada
OnFailure=, OnSuccess=
são scripts ou programas executáveis a serem acionados em caso de falha/sucesso na recepção. Note que serão passados dois parâmetros para os programas/scripts: o Bulk type e um texto
Output=
Nome do arquivo de saída. Assumido “BBBBJJJSS”no diretório corrente, com BBBB para o Bulk Type, JJJ para a data e SS para o n. de sequência
Recreate_File=
Indica se deve destruir o arquivo especificado em Output= se este existir no disco. Padrão: não recria
ReportFrequency=
Frequência em blocos entre cada relatório de status. Padrão: sem relatório automático. Veja abaixo um exemplo do relatório de status. Por exemplo, para ReportFrequency=20 um relatório de status é gerado automáticamente a cada 20 blocos recebidos do MIP. Teclando “enter” um relatório é emitido automaticamente em stderr
Sequence_Number=
Número de sequência do arquivo no MIP. Padrão: não envia número de sequência
SubNetwork=
Identificação do endereço de subnet para a conexão com o MIP
Time=
Tempo em segundos para aguardar uma resposta do MIP. Padrão: aguarda indefinidamente

Formato do arquivo de log
Quando usada a opção de debug todo o tráfego de mensagens entre o MIP e o host é gravado no arquivo de log, em formato texto, gerando grande quantidade de informação para testes, ou mesmo para familiarização com o processo de transferência. Milhares de linhas de log reais estão na pasta “LOG Files” no disco de distribuição. Seguem abaixo trechos do início e do final de uma transferência bem sucedida
Início de sessão de transferência
14/09/99 21:28:54:  ****** session             ******
14/09/99 21:28:54:  ******         starts      ******
14/09/99 21:28:54:  ******                here ******
14/09/99 21:28:54: Output File not in command line. OK
14/09/99 21:28:54: STANDARD Bulk File Transfer SET
14/09/99 21:28:54: Bulk File is T754
14/09/99 21:28:54: output file not specified
14/09/99 21:28:54: output file will be generated
14/09/99 21:28:54: SubNetwork id is "D"
14/09/99 21:28:54: Transfer status REPORT issued at each 5 Blocks
14/09/99 21:28:54: OUTPUT will be converted to ASCII
14/09/99 21:28:54: On reception failure "xfer_failure.sh" will run
14/09/99 21:28:54: On successfull reception "xfer_ok.sh" will run
14/09/99 21:28:54: Max LOG file SIZE set to 1000000
14/09/99 21:28:54: MIP Session
14/09/99 21:28:54: Bulk File T754 EndPoint 01940 Julian date is 251
14/09/99 21:28:54: Bulk Type is T
14/09/99 21:28:54: ATTACH ATTACH ATTACH ATTACH ATTACH
14/09/99 21:28:54: Starting File Receive
14/09/99 21:28:54: entering nliAttach()
14/09/99 21:28:54: Subnet: "D" Logical channel "2"
14/09/99 21:28:54: ATTACHED to PVC channel "2" subnetwork "D" fd is "5"
14/09/99 21:28:54: nliReset()
14/09/99 21:28:54: Logical Channel RESET (0)
14/09/99 21:28:54: Wait_For_Reset_Confirmation()
14/09/99 21:28:54: RESET confirmed (0)
14/09/99 21:28:54: REQUEST REQUEST REQUEST REQUEST REQUEST 

Bulk File Request ->
 EBCDIC Message: #19 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F1 F0 F1 F0 F1 E3 F7 F5 F4 F0 F1 F9 F4 F0 F2 F5 10101T75 40194025        
0016 F1 F4 40                                        14                       
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
14/09/99 21:28:54: entering nliSend(5)

About to send File Request ->
 EBCDIC Message: #19 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F1 F0 F1 F0 F1 E3 F7 F5 F4 F0 F1 F9 F4 F0 F2 F5 10101T75 40194025        
0016 F1 F4 40                                        14                       
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
14/09/99 21:28:54: Sending Message
14/09/99 21:28:54: nliSend() = 0
14/09/99 21:28:54: WAIT HDR WAIT HDR WAIT HDR WAIT HDR 
14/09/99 21:28:54: entering nliRecv(5,MAX=1024)
14/09/99 21:28:57: nliX25 getmsg ok [C=5,D=62] (R)
14/09/99 21:28:57: Buffer Type [C]
14/09/99 21:28:57: Command: N_Data
14/09/99 21:28:57: 62 bytes received
14/09/99 21:28:57: nliRecv returned 0, Len 62

Data from MIP ->
 EBCDIC Message: #62 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F0 F0 F4 F0 F1 E3 F7 F5 F4 F0 F1 F9 F4 F0 F2 F5 00401T75 40194025        
0016 F1 F4 40 00 00 00 00 00 00 00 00 00 00 00 00 00 14 ..... ........        
0032 00 00 00 00 00 00 00 90 00 00 00 00 00 00 00 00 ........ ........        
0048 00 00 00 00 00 00 00 00 00 00 00 00 00 00       ........ ......          
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
14/09/99 21:28:57: Bulk File Header Received
14/09/99 21:28:57: Entering Process_File_Header()
14/09/99 21:28:57: Total message length = 62
14/09/99 21:28:57: Output File is               
14/09/99 21:28:57: Output File is "T7542514"
14/09/99 21:28:57: File T7542514 is OPEN (6)
14/09/99 21:28:57: MIP REPORTED file BLOCK COUNT as 144 Blocks
14/09/99 21:28:57: File Transfer STARTED
14/09/99 21:28:57: RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE 
14/09/99 21:28:57: entering nliRecv(5,MAX=1024)
14/09/99 21:28:58: nliX25 getmsg ok [C=5,D=932] (R)
14/09/99 21:28:58: Buffer Type [C]
14/09/99 21:28:58: Command: N_Data
14/09/99 21:28:58: 932 bytes received

Data from MIP (2) ->
 EBCDIC Message: #932 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 E3 C7 C3 C8 C5 C1 C4 C5 D9 F7 F0 F9 F7 40 40 40 TGCHEADE R7097           
0016 40 40 40 40 40 40 40 F9 F9 F2 F5 F1 F2 F1 F5 F0        9 92512150        
0032 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0048 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0064 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0080 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0096 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0112 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0128 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0144 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0160 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0176 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0192 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0208 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0224 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0240 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0256 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0272 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0288 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0304 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0320 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0336 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0352 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0368 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0384 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0400 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0416 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0432 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0448 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0464 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0480 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0496 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0512 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0528 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0544 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0560 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0576 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0592 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0608 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0624 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0640 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0656 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0672 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0688 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0704 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0720 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0736 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0752 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0768 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0784 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0800 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0816 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0832 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0848 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0864 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0880 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0896 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0912 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0928 40 40 40 40                                                              
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
14/09/99 21:28:58: RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE 
14/09/99 21:28:58: entering nliRecv(5,MAX=1024)
14/09/99 21:28:59: nliX25 getmsg ok [C=5,D=932] (R)
14/09/99 21:28:59: Buffer Type [C]
14/09/99 21:28:59: Command: N_Data
14/09/99 21:28:59: 932 bytes received

Data from MIP (2) ->
 EBCDIC Message: #932 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 E3 F5 F2 F0 F0 F9 F9 F0 F9 F0 F8 F1 F3 F3 F1 F3 T5200990 90813313        
0016 F3 C3 40 40 40 40 40 40 40 40 40 40 40 40 40 40 3C                       
0032 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0048 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0064 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0080 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0096 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0112 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0128 40 40 40 40 40 40 F1 E2 E6 C3 C8 C4 F6 F3 40 40       1S WCHD63          
0144 40 40 5C 5C 5C C3 C5 D9 E3 C9 C6 C9 C3 C1 E3 C9   ***CER TIFICATI        
0160 D6 D5 40 E3 C5 E2 E3 C9 D5 C7 5C 5C 5C 40 40 40 ON TESTI NG***           
0176 40 40 40 D4 40 C1 40 E2 40 E3 40 C5 40 D9 40 C3    M A S  T E R C        
0192 40 C1 40 D9 40 C4 40 40 40 C4 40 C5 40 C2 40 C9  A R D    D E B I        
0208 40 E3 40 40 40 E2 40 E6 40 C9 40 E3 40 C3 40 C8  T   S W  I T C H        
0224 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0240 40 40 40 40 40 40 40 40 40 40 40 40 40 40 F8 F2                82        
0256 F7 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 7                        
0272 D9 C5 D7 D6 D9 E3 40 40 F4 40 40 40 40 40 40 40 REPORT   4               
0288 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0304 40 40 40 40 40 40 40 40 40 40 40 40 C4 40 C1 40              D A         
0320 C9 40 D3 40 E8 40 40 40 C3 40 D6 40 D5 40 E3 40 I L Y    C O N T         
0336 D9 40 D6 40 D3 40 40 40 D9 40 C5 40 D7 40 D6 40 R O L    R E P O         
0352 D9 40 E3 40 40 40 40 40 40 40 40 40 40 40 40 40 R T                      
0368 40 40 40 40 40 40 40 40 40 40 40 40 40 40 E6 D6                WO        
0384 D9 D2 40 D6 C6 40 40 F0 F9 61 F0 F8 61 F9 F9 40 RK OF  0 9/08/99         
0400 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0416 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0432 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 C1                 A        
0448 40 C3 40 D8 40 E4 40 C9 40 D9 40 C9 40 D5 40 C7  C Q U I  R I N G        
0464 40 40 40 D7 40 D9 40 D6 40 C3 40 C5 40 E2 40 E2    P R O  C E S S        
0480 40 D6 40 D9 40 40 40 40 40 40 40 40 40 40 40 40  O R                     
0496 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0512 40 40 40 40 40 40 40 40 40 40 40 D7 C1 C7 C5 40             PAGE         
0528 40 40 40 F1 40 40 40 40 40 40 40 40 40 40 40 D7    1            P        
0544 D9 D6 C3 C5 E2 E2 D6 D9 40 40 40 7A 40 E3 C5 C3 ROCESSOR    : TEC        
0560 C2 C1 D5 6B 40 C2 D9 C1 E9 C9 D3 40 40 40 40 40 BAN, BRA ZIL             
0576 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0592 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0608 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0624 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0640 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0656 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0672 40 40 40 40 D7 D9 D6 C3 C5 E2 E2 D6 D9 40 C9 C4     PROC ESSOR ID        
0688 7A 40 F9 F0 F0 F0 F0 F0 F0 F4 F2 F7 40 40 40 40 : 900000 0427            
0704 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0720 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0736 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0752 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0768 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0784 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 60                 -        
0800 40 E3 D9 C1 D5 E2 C1 C3 E3 C9 D6 D5 40 C4 C5 E2  TRANSAC TION DES        
0816 C3 D9 C9 D7 E3 C9 D6 D5 40 40 40 40 40 40 40 40 CRIPTION                 
0832 40 40 40 40 C6 C9 D5 C1 D5 C3 C9 C1 D3 40 40 40     FINA NCIAL           
0848 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0864 40 40 40 40 40 40 C6 C9 D5 C1 D5 C3 C9 C1 D3 40       FI NANCIAL         
0880 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0896 40 40 40 D9 C5 E5 C5 D9 E2 C1 D3 40 40 40 40 40    REVER SAL             
0912 40 40 40 40 40 40 40 40 40 40 D9 C5 E5 C5 D9 E2            REVERS        
0928 C1 D3 40 40                                     AL                       
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
14/09/99 21:28:59: Block Size is SET to 931
final da sessão
--.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
25/08/99 20:00:42: RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE 
25/08/99 20:00:42: entering nliRecv(5,MAX=1024)
25/08/99 20:00:43: nliX25 getmsg ok [C=5,D=841] (R)
25/08/99 20:00:43: Buffer Type [C]
25/08/99 20:00:43: Command: N_Data
25/08/99 20:00:43: 841 bytes received

Data from MIP (2) ->
 EBCDIC Message: #841 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 E3 E5 F4 F0 F3 F9 F8 F1 F4 F0 F3 F9 F8 F1 F0 F5 TV403981 40398105        
0016 F8 F2 F0 C2 C1 E2 C5 40 C6 C3 E4 40 40 40 40 40 820BASE  FCU             
0032 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0048 40 40 40 40 40 40 40 40 40 40 40 40 40 E5 F4 F8               V48        
0064 F1 F9 F8 F1 F4 F8 F1 F9 F8 F1 F0 F5 F8 F2 F0 E4 19814819 8105820U        
0080 D5 C9 E2 D6 D5 40 C6 C3 E4 40 40 40 40 40 40 40 NISON FC U               
0096 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0112 40 40 40 40 40 40 40 40 40 E5 F4 F8 F1 F9 F8 F2           V481982        
0128 F4 F8 F1 F9 F8 F2 F0 F5 F8 F2 F0 C6 C9 D9 C5 C6 48198205 820FIREF        
0144 C9 C7 C8 E3 C5 D9 E2 40 C6 C3 E4 40 40 40 40 40 IGHTERS  FCU             
0160 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0176 40 40 40 40 40 E5 F4 F0 F3 F9 F8 F3 F4 F0 F3 F9      V40 39834039        
0192 F8 F3 F0 F5 F8 F2 F0 C9 D5 C4 C9 C1 D5 C1 40 E2 8305820I NDIANA S        
0208 E3 C1 E3 C5 40 E4 D5 C9 E5 C5 D9 40 40 40 40 40 TATE UNI VER             
0224 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0240 40 E5 F4 F0 F3 F9 F8 F4 F4 F0 F3 F9 F8 F4 F0 F5  V403984 40398405        
0256 F8 F2 F0 D7 E2 C9 4D E3 4B C8 4B 40 C4 C9 E5 C9 820PSI(T .H. DIVI        
0272 E2 C9 D6 D5 5D 40 40 40 40 40 40 40 40 40 40 40 SION)                    
0288 40 40 40 40 40 40 40 40 40 40 40 40 40 E5 F4 F0               V40        
0304 F3 F9 F8 F5 F4 F0 F3 F9 F8 F5 F0 F5 F8 F2 F0 C3 39854039 8505820C        
0320 C5 D5 E3 E4 D9 C9 D6 D5 40 C6 C3 E4 40 40 40 40 ENTURION  FCU            
0336 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0352 40 40 40 40 40 40 40 40 40 E5 F4 F0 F3 F9 F8 F6           V403986        
0368 F4 F0 F3 F9 F8 F6 F0 F5 F8 F2 F0 C5 C1 E2 E3 40 40398605 820EAST         
0384 C4 C9 E5 C9 E2 C9 D6 D5 40 C6 C3 E4 40 40 40 40 DIVISION  FCU            
0400 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0416 40 40 40 40 40 E5 F4 F2 F0 F9 F8 F6 F4 F2 F0 F9      V42 09864209        
0432 F8 F6 F0 F1 F0 F6 F0 D5 C5 C2 C1 40 40 40 40 40 8601060N EBA             
0448 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0464 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0480 40 E5 F4 F0 F3 F9 F8 F8 F4 F0 F3 F9 F8 F8 F0 F5  V403988 40398805        
0496 F8 F2 F0 C1 C1 40 C6 C3 E4 40 40 40 40 40 40 40 820AA FC U               
0512 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0528 40 40 40 40 40 40 40 40 40 40 40 40 40 E5 F4 F2               V42        
0544 F0 F9 F9 F1 F4 F2 F0 F9 F9 F1 F0 F1 F0 F1 F5 E6 09914209 9101015W        
0560 E2 C2 C1 40 40 40 40 40 40 40 40 40 40 40 40 40 SBA                      
0576 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0592 40 40 40 40 40 40 40 40 40 E5 F4 F0 F0 F9 F0 F0           V400900        
0608 F4 F0 F0 F9 F9 F9 F0 F1 F0 F7 F0 C3 C5 D5 E3 D9 40099901 070CENTR        
0624 C1 D3 40 E3 D9 E4 E2 E3 40 C2 D2 40 40 40 40 40 AL TRUST  BK             
0640 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0656 40 40 40 40 40 E5 F4 F0 F9 F9 F0 F0 F4 F0 F9 F9      V40 99004099        
0672 F9 F9 F0 F1 F0 F7 F0 D4 C1 C7 D5 C1 40 C2 C1 D5 9901070M AGNA BAN        
0688 D2 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 K                        
0704 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0720 40 E5 F4 F3 F2 F9 F0 F0 F4 F3 F2 F9 F9 F9 F0 F1  V432900 43299901        
0736 F0 F6 F4 E2 C5 C2 C1 40 40 40 40 40 40 40 40 40 064SEBA                  
0752 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0768 40 40 40 40 40 40 40 40 40 40 40 40 40 E5 F4 F7               V47        
0784 F1 F9 F0 F0 F4 F7 F1 F9 F9 F9 F0 F1 F2 F8 F4 F0 19004719 99012840        
0800 F0 F6 F0 F0 F5 F0 40 40 40 40 40 40 40 40 40 40 060050                   
0816 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40                          
0832 40 40 40 40 40 40 40 40 40                                               
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
25/08/99 20:00:43: RECEIVE RECEIVE RECEIVE RECEIVE RECEIVE 
25/08/99 20:00:43: entering nliRecv(5,MAX=1024)
25/08/99 20:00:43: nliX25 getmsg ok [C=5,D=11] (R)
25/08/99 20:00:43: Buffer Type [C]
25/08/99 20:00:43: Command: N_Data
25/08/99 20:00:43: 11 bytes received

Data from MIP (2) ->
 EBCDIC Message: #11 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F9 F9 F8 F0 F1 F0 F0 00 00 03 C0                9980100. ..{             
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
25/08/99 20:00:43: ENDING ENDING ENDING ENDING ENDING 

Data from MIP ->
 EBCDIC Message: #11 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F9 F9 F8 F0 F1 F0 F0 00 00 03 C0                9980100. ..{             
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
25/08/99 20:00:43: MIP reports 960 blocks sent
25/08/99 20:00:43: HOST reports 960 blocks read
25/08/99 20:00:43: Sending PURGE REQUEST

PURGE request ->
 EBCDIC Message: #21 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F9 F9 F9 F0 F1 F0 F0 E3 F0 F0 F8 F0 F1 F9 F4 F0 9990100T 00801940        
0016 F2 F3 F2 F0 F3                                  23203                    
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
25/08/99 20:00:43: entering nliSend(5)

About to send File Request ->
 EBCDIC Message: #19 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F9 F9 F9 F0 F1 F0 F0 E3 F0 F0 F8 F0 F1 F9 F4 F0 9990100T 00801940        
0016 F2 F3 F2                                        232                      
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
25/08/99 20:00:43: Sending Message
25/08/99 20:00:43: CONFIRM PURGE CONFIRM PURGE 
25/08/99 20:00:43: entering nliRecv(5,MAX=1024)
25/08/99 20:00:44: nliX25 getmsg ok [C=5,D=11] (R)
25/08/99 20:00:44: Buffer Type [C]
25/08/99 20:00:44: Command: N_Data
25/08/99 20:00:44: 11 bytes received

PURGE request response ->
 EBCDIC Message: #11 bytes

     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15 01234567 89ABCDEF
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
0000 F9 F9 F8 F0 F1 F0 F0 00 00 00 00                9980100. ...             
     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.-- ........ ........
25/08/99 20:00:44: PURGE request confirmed
25/08/99 20:00:44: DETACH DETACH DETACH DETACH DETACH 
25/08/99 20:00:44: 920520 Bytes Received in 958 Blocks
25/08/99 20:00:44: PVC DETACH (0)
25/08/99 20:00:44: END END END END END 
25/08/99 20:00:44: bye
25/08/99 20:00:44: Completion Code for run is 0

A estrutura do programa
O programa xfer é gerado a partir de vários fontes C, e um makefile está incluído no disco de distribuição. Basicamente temos:
arfn1.h
header com as estruturas de dados usadas pelo produto
arfn1.c
APIs para controle e gravação do log e tratamento dos parâmetros
arfn2.c
APIs para interface com o MIP
nliX25.c
APIs para acesso à camada de interface com o protocolo X25
arfnx25.c
APIs relacionadas a um emulador do MIP, usado durante o desenvolvimento do programa
A biblioteca nliX25
Seguem os protótipos das APIs escritas para interface com o MIP


int nliWait_For_Reset_Confirmation( int * fd_x25 )
int nliReset( int * fd_x25 )
int nliDetach( int  *fd )
int nliAttach( int  *fd, char * Subnet, int Channel)
int nliSend( int * fd_x25, unsigned char * Buffer, int Len)
int nliDisplay_Command( char typ, unsigned char cmd, char * Control_Buffer )
int nliRecv( int * fd_x25, unsigned char * Buffer, int * Len)

Exemplos
Exemplo de um arquivo xfer.cfg
Bulk_File=T998
ConvertToASCII=Y
Endpoint=01940
ICA=7097
Julian_Date=232
Logical_Channel=1
MaxLogSize=1000000
OnFailure=xfer_failure.sh
OnSuccess=xfer_ok.sh
Output=mip
Recreate_File=Y
ReportFrequency=0
Sequence_Number=03
SubNetwork=F
Time=20
LogFileName=MIP.log

Exemplo do relatório de status
Transmit time: 20s
Blocks Transferred: 21 out of 959 (2.19%) [Block Length 960]
Bytes Transferred: 68944
Transferring 1.05 blocks/s (1008cps)
Estimated time to end 893.33s [14:53]

Exemplos da saída no modo advisement:
    Bulk ID        SIZE  DAY  EXPIRE  S  RECORD  RECEIVED
R001CNSYS23101     38230 231 1999/241 D 000/000 231-12:17:10
R001CNSYS23201     37892 232 1999/242 D 000/000 232-10:31:52
T0080194023201    920520 232 1999/242 P 960/060 232-14:00:18
T0080194023202    920520 232 1999/242 P 960/060 232-14:11:57
T0080194023203    920520 232 1999/242 S 960/060 232-14:22:51

/home/master: xfer -tAP advise

@(#) Versao MFT 1.0.0.1 1999 ARFN/Open
@(#) xfer.c 3.3 14 Sep 1999 16:58:48

Bulk ID        SIZE  DAY  EXPIRE  S  RECORD  RECEIVED
T701019402483       2886 248 1999/258 P 101/960 248-01:54:38
T701019402494       2886 249 1999/259 P 101/960 249-01:32:08
T701019402505       2886 250 1999/260 P 101/960 250-01:36:29
T701019402511       2886 251 1999/261 P 101/960 251-02:48:39
T754019402514     134352 251 1999/261 P 101/931 251-21:54:55
T754019402525      91434 252 1999/262 P 101/931 252-20:24:14
T754019402531      91434 253 1999/263 P 101/931 253-16:54:58
T754019402542     109161 254 1999/264 P 101/931 254-16:18:52
T754019402553      91434 255 1999/265 P 101/931 255-16:22:54
T755019402514      24050 251 1999/261 P 101/960 251-22:04:33
T755019402525    3597880 252 1999/262 P 101/960 252-20:25:01
T755019402542       6734 254 1999/264 P 101/960 254-16:28:30
T755019402553       2886 255 1999/265 P 101/960 255-16:32:29
T755019402571       9620 257 1999/267 P 101/960 257-13:16:28
Referências:

Banknet Systems Manual (Dec 1997 Ed.)

X.25Release 5 NLI Application interface (FTX documentation)

Distribuição

Pasta	Conteúdo
C Sources	Versão corrente de cada programa
Cpio	Bibliotecas no formato cpio com o ambiente Unix completo
Data	Arquivos de log de transferências e exemplos de alguns arquivos reais recebidos do MIP da Tecnologia Bancária
Docs	Alguns documentos, em especial este texto e o pequeno manual de referência xferqr, gerados pelo programa Word 97.
Headers	Para referência, headers da interface X25 do FTX
Sccs	Bibiotecas sccs com a evolução dos fontes
Templates	Templates para impressão de arquivos de log e programas fonte

