#!/usr/bin/env python
"""
Bootloader for dsPIC33FJ128MC706
author: stanbaek
version: 1.01

Modifications on 2010-01-12 (Ver 1.01): 
    COM ports larget than 9 could not be handled properly.
    Nowm this code can read two digit numbers from "COMXX" where 
    XX is the serial port for Windows
"""

import wx, time, os, sys
from serial import *
from scanf import sscanf
import array
import numpy as np
import bluetooth

####################################################
""" Modify the following three lines as you need """
####################################################
DEFAULT_SERIAL_PORT = '/dev/tty.STAN-9EDC-SPP'
DEFAULT_HEX_FILE = 'ImageProc.hex'
DEFAULT_HEX_DIR = ''


PM30F_ROW_SIZE = 32

CMD_NACK = chr(0x00)
CMD_ACK = chr(0x01)
CMD_READ_PM = chr(0x02)
CMD_WRITE_PM = chr(0x03)
CMD_READ_EE = chr(0x04)
CMD_WRITE_EE =chr(0x05)
CMD_READ_CM = chr(0x06)
CMD_WRITE_CM = chr(0x07)
CMD_RESET = chr(0x08)
CMD_READ_ID = chr(0x09)       
CMD_READ_GOTO = chr(0x10)

DEBUG = False

def debug(s):
    #if __debug__:
    if DEBUG:
        print s
    else:
        return

class FlashMem(object):
    
    # static class variables
    PROGRAM = 1
    EEPROM = 2
    CONFIG = 3
    
    PM_ROW_SIZE = 512

    PM_SIZE = 86  # Max: 128KB/3/512 = ~85.333 
    CM_SIZE = 8

    def __init__(self, startAddr, rowNumber, memtype):
        self.empty = True
        self.rowNumber = rowNumber
        self.type = memtype
        if memtype is self.PROGRAM:
            self.rowSize = self.PM_ROW_SIZE
            self.address = startAddr + rowNumber*self.PM_ROW_SIZE*2
        elif memtype is self.CONFIG:
            self.address = startAddr + rowNumber*2
        else:
            print "invalid memory type"


    def InsertData(self, address, data):

        # debug("... 0x%06x, 0x%06x" %(address, self.address) )

        if (address < self.address) or (address >= (self.address + self.PM_ROW_SIZE*2)):
            return False

        if self.empty is True:
            if self.type is self.PROGRAM:
                self.buffer = array.array('c', chr(0xff)*self.PM_ROW_SIZE*3)
            else:
                self.buffer = array.array('c', chr(0xff)*3)

            self.empty = False

        addr = address - self.address
        # debug( "address received: 0x%06x, self.address: 0x%06x" %(address, self.address))
        # debug("At 0x%06x, %s is inserted" %(address, data) )
        
        (value,) = sscanf(data, "%4x")

        if addr%2 is 0:
            self.buffer[(addr>>1)*3 + 0] = chr((value >> 8) & 0x00FF)
            self.buffer[(addr>>1)*3 + 1] = chr(value & 0x00FF)
        else:
            self.buffer[(addr>>1)*3 + 2] = chr((value >> 8) & 0x00FF)
            # print "Data at Address: 0x%x Data: 0x%02x%02x%02x" %(addr-1, 
            #         ord(self.buffer[(addr>>1)*3+2]), 
            #           ord(self.buffer[(addr>>1)*3+1]), ord(self.buffer[(addr>>1)*3]))

        return True


    def SendData(self, conn):

        if self.empty is True: 
            return
        ack = False
        debug("writing to 0x%06x" %(self.address))
    
        count = 0
        s = self.buffer.tostring()
        while not ack:
            try:
                conn.write(CMD_WRITE_PM)
                conn.write( chr(self.address & 0xFF) )
                conn.write( chr( (self.address >> 8) & 0xFF ) )
                conn.write( chr( (self.address >> 16) & 0xFF ) )

                # for i in range(self.ROW_SIZE*3):
                #     conn.write(self.buffer[i])
                # print self.buffer
        
                # print len(s)
                conn.write(s)
            except SerialTimeoutException, e:
                print "SerialTimeoutException" + e.message
            except SerialException, e:
                print "SerialExcaption" + e.message
            else:

                if (conn.read() == CMD_ACK):
                    debug( "ACK")
                    ack = True
                else:
                    print "NACK: Trying again...."
                    ack = False
                    count += 1
                    if count == 5:
                        print "COMMUNICATION FAILURE: too many NACKs received"
                        return False
        
        #debug("written to 0x%06x" %(self.address))
        return True


    def PrintData(self, fout):

        if self.empty is True: 
            return

        addr = 0
        for i in range(self.PM_ROW_SIZE):
            fout.write("0x%06x: %02x%02x%02x\n" %(self.address + addr, 
                        ord(self.buffer[3*i+2]), ord(self.buffer[3*i+1]), ord(self.buffer[3*i])))
            addr = addr + 2



class Bootloader(object):

    DEVICE_ID = ("dsPIC33FJ128MC706", 0xA9, 3, "dsPIC33F")

    def __init__(self, gui):
        self.device = self.DEVICE_ID
        self.gui = gui
        self.conn = None
        self.hexFile = None
        self.memory = np.array([object]*FlashMem.PM_SIZE, 'O')
        self.confmem = np.array([object]*FlashMem.CM_SIZE, 'O')

        # initialize memory
        for row in range(FlashMem.PM_SIZE):
            self.memory[row] = FlashMem(0x000000, row, FlashMem.PROGRAM)

        for row in range(FlashMem.CM_SIZE):
            self.confmem[row] = FlashMem(0xF80000, row, FlashMem.CONFIG)

    def Close(self):
        self.Disconnect()


    def Run(self, port):

        if self.conn is None:
            try:
                self.conn = bluetooth.Bluetooth(port, 230400, timeout=1, rtscts=1)
                self.conn.writeTimeout = 10
            except:
                print "ERROR: Could not establish a connection to the Bluetooth device."
                self.conn = None

        if self.conn.handshake():
            self.conn.write(CMD_NACK)
            return True
        else:
            print "handshake failed"
            return False


    def Connect(self, port):
        if self.conn is not None:
            return True
       
        try:
            self.conn = bluetooth.Bluetooth(port, 230400, timeout=1, rtscts=1)
            self.conn.writeTimeout = 10
        except SerialException, e:
            print "SerialException" + e.message
            self.conn = None
            return False
        
        return self.conn.handshake()


    def Disconnect(self):
        try:
            self.conn.close()
        except SerialException:
            pass   
        else:
            self.conn = None
            return True

    def ReadID(self):
        if self.conn is None:
            return "Connection failed"
    
        self.conn.write(CMD_READ_ID)
        buffer = self.conn.read(8)
        
        deviceID = ((ord(buffer[1]) << 8) & 0xFF00) | (ord(buffer[0]) & 0x00FF)
        processID = (ord(buffer[5]) >> 4) & 0x0F
    
        if (deviceID == self.DEVICE_ID[1] and processID == self.DEVICE_ID[2]):
            return self.DEVICE_ID[0]
        else:
            return "Unknown device!"


    def ReadGOTO(self):
        print "Reading GOTO block"
        self.conn.write(CMD_READ_GOTO)
        #time.sleep(1)
        buffer = self.conn.read(6)
        return buffer

    def ReplaceReset(self):
            
        self.memory[0].InsertData(0x000000, "0004")
        self.memory[0].InsertData(0x000001, "0400")
        self.memory[0].InsertData(0x000002, "0000")
        self.memory[0].InsertData(0x000003, "0000")


    def PrintData(self):

        fout = open("hexout.txt", 'w')
        for row in range(FlashMem.PM_SIZE):
            self.memory[row].PrintData(fout)

        for row in range(FlashMem.CM_SIZE):
            self.confmem[row].PrintData(fout)

        fout.close()

    def Download(self):
    
        conn = self.conn

        print "Prgramming Flash Memory"

        for row in range(FlashMem.PM_SIZE):
            debug("Row #: %d " %(row) )
            self.memory[row].SendData(conn)

        debug( "writing RESET command.")
        conn.write(CMD_RESET)

        time.sleep(.1)
        print "Done."



    def ReadProgMemory(self, addr):
        rowSize = FlashMem.PM_ROW_SIZE      # 33F PM row size 
        readAddr = addr - addr % (rowSize*2)
        self.conn.write(CMD_READ_PM)
        self.conn.write(chr(readAddr & 0xFF))
        self.conn.write(chr((readAddr >> 8) & 0xFF))
        self.conn.write(chr((readAddr >> 16) & 0xFF))
        time.sleep(1)
        buffer = self.conn.read(rowSize*3)
        count = 0
        while (count < rowSize*3):
            text = "0x%06x: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x" %(readAddr, 
                    ord(buffer[count]), ord(buffer[count+1]), ord(buffer[count+2]),
                    ord(buffer[count+3]), ord(buffer[count+4]), ord(buffer[count+5]),
                    ord(buffer[count+6]), ord(buffer[count+7]), ord(buffer[count+8]),
                    ord(buffer[count+9]), ord(buffer[count+10]), ord(buffer[count+11]) )
            #self.gui.println(text)
            print text
            count = count + 12
            readAddr = readAddr + 8


    def OpenFile(self, filename):
        try:   
            self.file = open(filename,'r')
        except IOError:
            print 'ERROR: File cannot be open.'


    def CloseFile(self):
        try:
            self.file.close()
        except IOError:
            print "ERROR: File cannot be closed." 

    def ReadHexFile(self):
    
        print "Reading HexFile"
        extAddr = 0
        lines = self.file.readlines()

        for line in lines:
            (byteCount, addr, recordType) = sscanf(line[1:], "%2x%4x%2x")

            if (recordType == 0):
                addr = (addr + extAddr) / 2
                for count in range(0, byteCount*2, 4):
                    if addr < 0xF80000:
                        bInserted = self.memory[addr/(FlashMem.PM_ROW_SIZE*2)].InsertData(addr,line[9+count:13+count])
                    else:
                        print "Configuration Codes - Address: 0x%x, Data: %s" %(addr, line[9+count:13+count])
 
                    if bInserted is True:
                        #print "Hex File: 0x%x Address: %s" %(addr, line[9+count:13+count])
                        addr = addr + 1
                    else:
                        print "Bad HexFile: 0x%x Address out of range: %s" %(addr, 
                            line[9+count:13+count]) 

            elif (recordType == 1):
                "It means EOF, just do nothing"
            
            elif (recordType == 4):
                extAddr = sscanf(line[9:], "%4x")
                extAddr = extAddr[0] << 16

            else:
                print "Unknown hex record type"
                return


class BootloaderGUI(wx.Frame):

    ID_QUIT = 0
    ID_STAT = 1
    ID_TOOL = 2


    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title, size = (480, 350))

        self.InitLayout()
        self.bootloader = Bootloader(self)
        self.connected = False

    def InitLayout(self):

        ####################################
        #   Main panel layout
        ####################################
        panel = wx.Panel(self, -1)
        sizer = wx.GridBagSizer(0,0)

        text1 = wx.StaticText(panel, -1, "Hex File")
        sizer.Add(text1, (0, 0), (1,1), wx.LEFT|wx.TOP|wx.ALIGN_CENTER, 10)

        self.fileTextCtrl = wx.TextCtrl(panel, -1)
        sizer.Add(self.fileTextCtrl, (0, 1), (1, 3), wx.TOP|wx.LEFT|wx.EXPAND, 10)

        btnBrowse = wx.Button(panel, -1, 'Browse')
        self.Bind(wx.EVT_BUTTON, self.OnClickBrowse, btnBrowse)
        sizer.Add(btnBrowse,(0, 4), (1,1), wx.TOP|wx.LEFT|wx.RIGHT, 10)

        bsizer = wx.StaticBoxSizer(wx.StaticBox(panel, -1, 'Options'), wx.VERTICAL)
        self.ICD2Chkbox = wx.CheckBox(panel, -1, 'ICD 2 triggered')
        self.ICD2Chkbox.SetValue(False)
        bsizer.Add(self.ICD2Chkbox, 0, wx.LEFT|wx.TOP, 5)

        self.ConfigChkbox = wx.CheckBox(panel, -1, 'Write Configuration Memory (Not Implemented)')
        self.ConfigChkbox.SetValue(False)
        bsizer.Add(self.ConfigChkbox, 0, wx.LEFT|wx.TOP|wx.BOTTOM, 5)


        sizer.Add(bsizer, (1,0), (1,5), wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT, 10)


        btnConnect = wx.Button(panel, -1, 'Connect')
        self.Bind(wx.EVT_BUTTON, self.OnClickConnect, btnConnect)
        sizer.Add(btnConnect, (2,0), (1,1), wx.LEFT|wx.TOP, 10)

        btnDisconnect = wx.Button(panel, -1, 'Disconnect')
        self.Bind(wx.EVT_BUTTON, self.OnClickDisconnect, btnDisconnect)
        sizer.Add(btnDisconnect, (2,1), (1,1), wx.LEFT|wx.TOP, 10)

        self.serialName = wx.TextCtrl(panel, -1)
        self.serialName.SetValue(DEFAULT_SERIAL_PORT)
        sizer.Add(self.serialName, (2,2), (1,3), wx.EXPAND|wx.LEFT|wx.TOP|wx.RIGHT, 10)

        self.PrintBox = wx.ListBox(panel, -1, size = (250, 100), style = wx.EXPAND)
        sizer.Add(self.PrintBox, (3,0), (1, 5), wx.EXPAND|wx.LEFT|wx.TOP|wx.RIGHT, 10)


        btnDownload = wx.Button(panel, -1, 'Download')
        self.Bind(wx.EVT_BUTTON, self.OnClickDownload, btnDownload)
        sizer.Add(btnDownload, (4,3), (1,1), wx.LEFT|wx.TOP|wx.BOTTOM, 10)

        btnAbort = wx.Button(panel, -1, 'Abort')
        self.Bind(wx.EVT_BUTTON, self.OnClickAbort, btnAbort)
        sizer.Add(btnAbort, (4,4), (1,1), wx.LEFT|wx.TOP|wx.BOTTOM, 10)

        sizer.AddGrowableCol(2)
        sizer.AddGrowableRow(3)
        panel.SetSizer(sizer)


        ####################################
        #   Statusbar
        ####################################
        self.statusbar = self.CreateStatusBar()
        self.statusFields = ['Disconnected','No device','']
        self.statusbar.SetFields(self.statusFields)
       

        ####################################
        # Events
        ####################################
        self.Bind(wx.EVT_CLOSE, self.OnClose)


        self.Center()
        self.Show(True)
        #self.SetFocus()


    def OnClickAbort(self, evt):
        "not implemented"

    def OnClickDownload(self, evt):
        try:   
            filename = os.path.join(self.dirname, self.filename)
        except:
            dlg = wx.MessageDialog(self, 
                    'Invalid file name or path.', 
                    'Error',
                    wx.OK | wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
        else:
            self.bootloader.OpenFile(filename)
            self.bootloader.ReadHexFile()
            self.bootloader.ReplaceReset()
            self.bootloader.CloseFile()
            self.bootloader.PrintData()
            self.bootloader.Download()

        self.SetFocus()           



    def OnClickBrowse(self, evt):
        self.dirname = DEFAULT_HEX_DIR
        dlg = wx.FileDialog(self, "Choose a file", self.dirname, "", "*.hex", wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            self.filename=dlg.GetFilename()
            self.dirname=dlg.GetDirectory()
            self.fileTextCtrl.SetValue(self.dirname + self.filename)
        dlg.Destroy()

    def OnClickDisconnect(self, evt):
        if self.bootloader.Disconnect():
            self.statusFields[0] = 'Disconnected'  
            self.statusbar.SetFields(self.statusFields)
            self.println("Bluetooth connection lost.")
        else:
            self.println("Bluetooth disconnection error.")

    def OnClickConnect(self, evt):
        self.portName = self.serialName.GetValue()

        if sys.platform == 'win32':
            port = int(self.portName[3:]) - 1     # e.g., port = 5 for COM6
        else:
            port = self.portName

        if self.bootloader.Connect(port):
            self.statusFields[0] = 'Connected'  
            device = self.bootloader.ReadID()
            self.statusFields[1] = device
            self.statusbar.SetFields(self.statusFields)
        else:
            dlg = wx.MessageDialog(self, 
                'Could not establish a connection to the Bluetooth device.', 
                'Error', wx.OK | wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()

    def println(self, line):
        self.PrintBox.Append(line)
        self.PrintBox.ScrollLines(1)

    def OnClose(self, event):
        dial = wx.MessageDialog(None, 'Are you sure to quit?', 'Question', 
                wx.YES_NO | wx.NO_DEFAULT | wx.ICON_QUESTION)
        ret = dial.ShowModal()
        if ret == wx.ID_YES:
            try:
                self.bootloader.Close()
            except:
                pass
            self.Destroy()
        else:
            event.Veto()

    def OnExit(self, event):
        self.OnClose()


if __name__ == '__main__':
    app = wx.App()
    BootloaderGUI(None, -1, 'Bootloader for dsPIC33FJ128MC706')
    app.MainLoop()

