#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/system_config/default/framework/driver/i2c/src/drv_i2c_static_buffer_model.c ../src/system_config/default/framework/driver/i2c/src/drv_i2c_mapping.c ../src/system_config/default/framework/driver/oc/src/drv_oc_mapping.c ../src/system_config/default/framework/driver/oc/src/drv_oc_static.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_mapping.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_tasks.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_sys_queue.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_rm_tasks.c ../src/system_config/default/framework/driver/tmr/src/drv_tmr_static.c ../src/system_config/default/framework/driver/tmr/src/drv_tmr_mapping.c ../src/system_config/default/framework/driver/usart/src/drv_usart_mapping.c ../src/system_config/default/framework/driver/usart/src/drv_usart_static.c ../src/system_config/default/framework/driver/usart/src/drv_usart_static_byte_model.c ../src/system_config/default/framework/system/clk/src/sys_clk_pic32mx.c ../src/system_config/default/framework/system/devcon/src/sys_devcon.c ../src/system_config/default/framework/system/devcon/src/sys_devcon_pic32mx.c ../src/system_config/default/framework/system/ports/src/sys_ports_static.c ../src/system_config/default/system_init.c ../src/system_config/default/system_interrupt.c ../src/system_config/default/system_exceptions.c ../src/system_config/default/system_tasks.c ../src/app.c ../src/main.c ../src/PIC_VAR.c ../src/PIC_MF40.c ../src/PIC_FT8X1_MF40.c ../src/PIC_MODBUS.c ../src/PIC_Func.c ../../../../microchip/harmony/v2_06/framework/system/int/src/sys_int_pic32.c ../../../../microchip/harmony/v2_06/framework/system/wdt/src/sys_wdt.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o ${OBJECTDIR}/_ext/715571337/drv_spi_static.o ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o ${OBJECTDIR}/_ext/327000265/drv_usart_static.o ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o ${OBJECTDIR}/_ext/340578644/sys_devcon.o ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o ${OBJECTDIR}/_ext/822048611/sys_ports_static.o ${OBJECTDIR}/_ext/1688732426/system_init.o ${OBJECTDIR}/_ext/1688732426/system_interrupt.o ${OBJECTDIR}/_ext/1688732426/system_exceptions.o ${OBJECTDIR}/_ext/1688732426/system_tasks.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o ${OBJECTDIR}/_ext/1360937237/PIC_Func.o ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o ${OBJECTDIR}/_ext/496838821/sys_wdt.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o.d ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o.d ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o.d ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o.d ${OBJECTDIR}/_ext/715571337/drv_spi_static.o.d ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o.d ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o.d ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o.d ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o.d ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o.d ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o.d ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o.d ${OBJECTDIR}/_ext/327000265/drv_usart_static.o.d ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o.d ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o.d ${OBJECTDIR}/_ext/340578644/sys_devcon.o.d ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o.d ${OBJECTDIR}/_ext/822048611/sys_ports_static.o.d ${OBJECTDIR}/_ext/1688732426/system_init.o.d ${OBJECTDIR}/_ext/1688732426/system_interrupt.o.d ${OBJECTDIR}/_ext/1688732426/system_exceptions.o.d ${OBJECTDIR}/_ext/1688732426/system_tasks.o.d ${OBJECTDIR}/_ext/1360937237/app.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o.d ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o.d ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o.d ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o.d ${OBJECTDIR}/_ext/1360937237/PIC_Func.o.d ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o.d ${OBJECTDIR}/_ext/496838821/sys_wdt.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o ${OBJECTDIR}/_ext/715571337/drv_spi_static.o ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o ${OBJECTDIR}/_ext/327000265/drv_usart_static.o ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o ${OBJECTDIR}/_ext/340578644/sys_devcon.o ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o ${OBJECTDIR}/_ext/822048611/sys_ports_static.o ${OBJECTDIR}/_ext/1688732426/system_init.o ${OBJECTDIR}/_ext/1688732426/system_interrupt.o ${OBJECTDIR}/_ext/1688732426/system_exceptions.o ${OBJECTDIR}/_ext/1688732426/system_tasks.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o ${OBJECTDIR}/_ext/1360937237/PIC_Func.o ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o ${OBJECTDIR}/_ext/496838821/sys_wdt.o

# Source Files
SOURCEFILES=../src/system_config/default/framework/driver/i2c/src/drv_i2c_static_buffer_model.c ../src/system_config/default/framework/driver/i2c/src/drv_i2c_mapping.c ../src/system_config/default/framework/driver/oc/src/drv_oc_mapping.c ../src/system_config/default/framework/driver/oc/src/drv_oc_static.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_mapping.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_tasks.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_sys_queue.c ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_rm_tasks.c ../src/system_config/default/framework/driver/tmr/src/drv_tmr_static.c ../src/system_config/default/framework/driver/tmr/src/drv_tmr_mapping.c ../src/system_config/default/framework/driver/usart/src/drv_usart_mapping.c ../src/system_config/default/framework/driver/usart/src/drv_usart_static.c ../src/system_config/default/framework/driver/usart/src/drv_usart_static_byte_model.c ../src/system_config/default/framework/system/clk/src/sys_clk_pic32mx.c ../src/system_config/default/framework/system/devcon/src/sys_devcon.c ../src/system_config/default/framework/system/devcon/src/sys_devcon_pic32mx.c ../src/system_config/default/framework/system/ports/src/sys_ports_static.c ../src/system_config/default/system_init.c ../src/system_config/default/system_interrupt.c ../src/system_config/default/system_exceptions.c ../src/system_config/default/system_tasks.c ../src/app.c ../src/main.c ../src/PIC_VAR.c ../src/PIC_MF40.c ../src/PIC_FT8X1_MF40.c ../src/PIC_MODBUS.c ../src/PIC_Func.c ../../../../microchip/harmony/v2_06/framework/system/int/src/sys_int_pic32.c ../../../../microchip/harmony/v2_06/framework/system/wdt/src/sys_wdt.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX350F256H
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o: ../src/system_config/default/framework/driver/i2c/src/drv_i2c_static_buffer_model.c  .generated_files/flags/default/67a0d17652e792e696089157671c7e9e114e0711 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/12144542" 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o.d 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o.d" -o ${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o ../src/system_config/default/framework/driver/i2c/src/drv_i2c_static_buffer_model.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o: ../src/system_config/default/framework/driver/i2c/src/drv_i2c_mapping.c  .generated_files/flags/default/163f487c10aa7a9d3a40942719dcd3379da02cf5 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/12144542" 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o.d" -o ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o ../src/system_config/default/framework/driver/i2c/src/drv_i2c_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o: ../src/system_config/default/framework/driver/oc/src/drv_oc_mapping.c  .generated_files/flags/default/bfe51cc069516baae635d6f15500efa7dcdf5363 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1047219354" 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o.d" -o ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o ../src/system_config/default/framework/driver/oc/src/drv_oc_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1047219354/drv_oc_static.o: ../src/system_config/default/framework/driver/oc/src/drv_oc_static.c  .generated_files/flags/default/41bb594b4655c17f89424efb8c5885259e743483 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1047219354" 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1047219354/drv_oc_static.o.d" -o ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o ../src/system_config/default/framework/driver/oc/src/drv_oc_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static.c  .generated_files/flags/default/7281728725509dd12d5805ef3dca597ef776766d .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_mapping.c  .generated_files/flags/default/3e1d5de1cda63589b9c703a71e2593af4dddccb2 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_tasks.c  .generated_files/flags/default/de7a82cd7bf4e65270b267cb482e60bbc4de858f .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_tasks.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_sys_queue.c  .generated_files/flags/default/f41e6ad3c730b94eb4f8ce7c7d3dca8dbe73fa47 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_sys_queue.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_rm_tasks.c  .generated_files/flags/default/712cbafcb4a1570aa4e2304c0225b1f3a588b2b0 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_rm_tasks.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o: ../src/system_config/default/framework/driver/tmr/src/drv_tmr_static.c  .generated_files/flags/default/3c10d6b188909e48c63b95efdd5e82fa010537d2 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1407244131" 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o.d" -o ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o ../src/system_config/default/framework/driver/tmr/src/drv_tmr_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o: ../src/system_config/default/framework/driver/tmr/src/drv_tmr_mapping.c  .generated_files/flags/default/6e76cc959e6ac2e58dbcdac25282c3e2cc118534 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1407244131" 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o.d" -o ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o ../src/system_config/default/framework/driver/tmr/src/drv_tmr_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o: ../src/system_config/default/framework/driver/usart/src/drv_usart_mapping.c  .generated_files/flags/default/2f2ac244f53b89e6bbe413140a19f36b7a2cdb3e .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/327000265" 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o.d" -o ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o ../src/system_config/default/framework/driver/usart/src/drv_usart_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/327000265/drv_usart_static.o: ../src/system_config/default/framework/driver/usart/src/drv_usart_static.c  .generated_files/flags/default/ba8e6fc624f6ada459955d4f98e426642b33c2fc .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/327000265" 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/327000265/drv_usart_static.o.d" -o ${OBJECTDIR}/_ext/327000265/drv_usart_static.o ../src/system_config/default/framework/driver/usart/src/drv_usart_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o: ../src/system_config/default/framework/driver/usart/src/drv_usart_static_byte_model.c  .generated_files/flags/default/ceef22cd02e11b6c86f507c9b83aa88ec5f873f2 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/327000265" 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o.d 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o.d" -o ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o ../src/system_config/default/framework/driver/usart/src/drv_usart_static_byte_model.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o: ../src/system_config/default/framework/system/clk/src/sys_clk_pic32mx.c  .generated_files/flags/default/a11688262a6be2a699c2cee654f4c12c3c812a49 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/639803181" 
	@${RM} ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o.d 
	@${RM} ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o.d" -o ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o ../src/system_config/default/framework/system/clk/src/sys_clk_pic32mx.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/340578644/sys_devcon.o: ../src/system_config/default/framework/system/devcon/src/sys_devcon.c  .generated_files/flags/default/9c37584a44b6b4c1ea148efcc13886efe4c741b .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/340578644" 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon.o.d 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/340578644/sys_devcon.o.d" -o ${OBJECTDIR}/_ext/340578644/sys_devcon.o ../src/system_config/default/framework/system/devcon/src/sys_devcon.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o: ../src/system_config/default/framework/system/devcon/src/sys_devcon_pic32mx.c  .generated_files/flags/default/10e2878371e83afc4d52ee5d85c18ff8b8fa489b .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/340578644" 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o.d 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o.d" -o ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o ../src/system_config/default/framework/system/devcon/src/sys_devcon_pic32mx.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/822048611/sys_ports_static.o: ../src/system_config/default/framework/system/ports/src/sys_ports_static.c  .generated_files/flags/default/3a69ae17211903022a602f0b94b79c87df4530d6 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/822048611" 
	@${RM} ${OBJECTDIR}/_ext/822048611/sys_ports_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/822048611/sys_ports_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/822048611/sys_ports_static.o.d" -o ${OBJECTDIR}/_ext/822048611/sys_ports_static.o ../src/system_config/default/framework/system/ports/src/sys_ports_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_init.o: ../src/system_config/default/system_init.c  .generated_files/flags/default/f97ab3303203e7915fade9b21a165e460d56d282 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_init.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_init.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_init.o ../src/system_config/default/system_init.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_interrupt.o: ../src/system_config/default/system_interrupt.c  .generated_files/flags/default/f8203806f5f3f381e809f814085e672ab7da019c .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_interrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_interrupt.o ../src/system_config/default/system_interrupt.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_exceptions.o: ../src/system_config/default/system_exceptions.c  .generated_files/flags/default/981a74d04fde3e998cc06bc5ba1114336aa30751 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_exceptions.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_exceptions.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_exceptions.o ../src/system_config/default/system_exceptions.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_tasks.o: ../src/system_config/default/system_tasks.c  .generated_files/flags/default/d95164ff2d2dc99131c18ab3392f9d6cb4025bb2 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_tasks.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_tasks.o ../src/system_config/default/system_tasks.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  .generated_files/flags/default/e24519c282640939bd7132795a88e30605656bb2 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/default/b3a6c2ac062d419d53ce685522df5a6868459dce .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_VAR.o: ../src/PIC_VAR.c  .generated_files/flags/default/1dcf6d5c086087e709daf0362763bb0d83418dc9 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_VAR.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o ../src/PIC_VAR.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_MF40.o: ../src/PIC_MF40.c  .generated_files/flags/default/2fc61c729581473983da6c172b78e929ea4ef366 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_MF40.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o ../src/PIC_MF40.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o: ../src/PIC_FT8X1_MF40.c  .generated_files/flags/default/c1ee4d4be8ef4223208effa64f00af19ee868ff5 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o ../src/PIC_FT8X1_MF40.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o: ../src/PIC_MODBUS.c  .generated_files/flags/default/7b2c27dca4f39c6a58b29b36ac985dfa3699b70f .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o ../src/PIC_MODBUS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_Func.o: ../src/PIC_Func.c  .generated_files/flags/default/23f74191a2f758fa9c13d1cef1eb350dc9964458 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_Func.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_Func.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_Func.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_Func.o ../src/PIC_Func.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/249303043/sys_int_pic32.o: ../../../../microchip/harmony/v2_06/framework/system/int/src/sys_int_pic32.c  .generated_files/flags/default/70cee2f215f94bf58407542f423830e3a4a138ee .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/249303043" 
	@${RM} ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o.d 
	@${RM} ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/249303043/sys_int_pic32.o.d" -o ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o ../../../../microchip/harmony/v2_06/framework/system/int/src/sys_int_pic32.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/496838821/sys_wdt.o: ../../../../microchip/harmony/v2_06/framework/system/wdt/src/sys_wdt.c  .generated_files/flags/default/edf22f079e1ef3502ee79e3f6a056b2a8e141bae .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/496838821" 
	@${RM} ${OBJECTDIR}/_ext/496838821/sys_wdt.o.d 
	@${RM} ${OBJECTDIR}/_ext/496838821/sys_wdt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/496838821/sys_wdt.o.d" -o ${OBJECTDIR}/_ext/496838821/sys_wdt.o ../../../../microchip/harmony/v2_06/framework/system/wdt/src/sys_wdt.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
else
${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o: ../src/system_config/default/framework/driver/i2c/src/drv_i2c_static_buffer_model.c  .generated_files/flags/default/49db98fad7a450f5cb32e748fa67bf0700097f31 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/12144542" 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o.d 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o.d" -o ${OBJECTDIR}/_ext/12144542/drv_i2c_static_buffer_model.o ../src/system_config/default/framework/driver/i2c/src/drv_i2c_static_buffer_model.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o: ../src/system_config/default/framework/driver/i2c/src/drv_i2c_mapping.c  .generated_files/flags/default/ff51ad678b45c8bd0ae39c06e989bac8972ea4d7 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/12144542" 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o.d" -o ${OBJECTDIR}/_ext/12144542/drv_i2c_mapping.o ../src/system_config/default/framework/driver/i2c/src/drv_i2c_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o: ../src/system_config/default/framework/driver/oc/src/drv_oc_mapping.c  .generated_files/flags/default/f29df5a24c60a8654bc8c74d1b26961b43cdbcaf .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1047219354" 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o.d" -o ${OBJECTDIR}/_ext/1047219354/drv_oc_mapping.o ../src/system_config/default/framework/driver/oc/src/drv_oc_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1047219354/drv_oc_static.o: ../src/system_config/default/framework/driver/oc/src/drv_oc_static.c  .generated_files/flags/default/b4c2a447d7dafd368c5efe87118629208b002e44 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1047219354" 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1047219354/drv_oc_static.o.d" -o ${OBJECTDIR}/_ext/1047219354/drv_oc_static.o ../src/system_config/default/framework/driver/oc/src/drv_oc_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static.c  .generated_files/flags/default/d4af566f7d7cf12379e97feb693b2d645b7c79ad .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_mapping.c  .generated_files/flags/default/1452c6c026fa565deef0fb89000eef45a2889a73 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_mapping.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_tasks.c  .generated_files/flags/default/9ff95f1cf2ab292ee0f1b0a06d128e367c6187a .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static_tasks.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_tasks.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_sys_queue.c  .generated_files/flags/default/c024196b377dd81f9b7cc7b7d153d2dd546dbcc4 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static_sys_queue.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_sys_queue.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o: ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_rm_tasks.c  .generated_files/flags/default/9375119966ba6fc4ab987b975ab6374091526a50 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/715571337" 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o.d" -o ${OBJECTDIR}/_ext/715571337/drv_spi_static_rm_tasks.o ../src/system_config/default/framework/driver/spi/static/src/drv_spi_static_rm_tasks.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o: ../src/system_config/default/framework/driver/tmr/src/drv_tmr_static.c  .generated_files/flags/default/8d3d112b83797d021443626646283780bb0ca366 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1407244131" 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o.d" -o ${OBJECTDIR}/_ext/1407244131/drv_tmr_static.o ../src/system_config/default/framework/driver/tmr/src/drv_tmr_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o: ../src/system_config/default/framework/driver/tmr/src/drv_tmr_mapping.c  .generated_files/flags/default/53a20468ee4050964b9d4d75419ad95c473e8690 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1407244131" 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o.d" -o ${OBJECTDIR}/_ext/1407244131/drv_tmr_mapping.o ../src/system_config/default/framework/driver/tmr/src/drv_tmr_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o: ../src/system_config/default/framework/driver/usart/src/drv_usart_mapping.c  .generated_files/flags/default/eb8dbff8b234bc74dbde344f99e3e5f10dde92f6 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/327000265" 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o.d" -o ${OBJECTDIR}/_ext/327000265/drv_usart_mapping.o ../src/system_config/default/framework/driver/usart/src/drv_usart_mapping.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/327000265/drv_usart_static.o: ../src/system_config/default/framework/driver/usart/src/drv_usart_static.c  .generated_files/flags/default/6ffaa985e9f24941316d2082a794ddd34c0af21b .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/327000265" 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/327000265/drv_usart_static.o.d" -o ${OBJECTDIR}/_ext/327000265/drv_usart_static.o ../src/system_config/default/framework/driver/usart/src/drv_usart_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o: ../src/system_config/default/framework/driver/usart/src/drv_usart_static_byte_model.c  .generated_files/flags/default/569d47dd824f7d6fdf75b0fa745585db2e8adbfa .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/327000265" 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o.d 
	@${RM} ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o.d" -o ${OBJECTDIR}/_ext/327000265/drv_usart_static_byte_model.o ../src/system_config/default/framework/driver/usart/src/drv_usart_static_byte_model.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o: ../src/system_config/default/framework/system/clk/src/sys_clk_pic32mx.c  .generated_files/flags/default/68b1eda2217b178bb38d6f4b64ade69c2028cd58 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/639803181" 
	@${RM} ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o.d 
	@${RM} ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o.d" -o ${OBJECTDIR}/_ext/639803181/sys_clk_pic32mx.o ../src/system_config/default/framework/system/clk/src/sys_clk_pic32mx.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/340578644/sys_devcon.o: ../src/system_config/default/framework/system/devcon/src/sys_devcon.c  .generated_files/flags/default/f5127d73a68e07a289cc679e1d8a0383bd960261 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/340578644" 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon.o.d 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/340578644/sys_devcon.o.d" -o ${OBJECTDIR}/_ext/340578644/sys_devcon.o ../src/system_config/default/framework/system/devcon/src/sys_devcon.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o: ../src/system_config/default/framework/system/devcon/src/sys_devcon_pic32mx.c  .generated_files/flags/default/87a0d41f24bf3a137c127860eb62d0c6082ecf7b .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/340578644" 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o.d 
	@${RM} ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o.d" -o ${OBJECTDIR}/_ext/340578644/sys_devcon_pic32mx.o ../src/system_config/default/framework/system/devcon/src/sys_devcon_pic32mx.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/822048611/sys_ports_static.o: ../src/system_config/default/framework/system/ports/src/sys_ports_static.c  .generated_files/flags/default/b13913b652d72f2c06fc3d5c18d0a634f8ea87e1 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/822048611" 
	@${RM} ${OBJECTDIR}/_ext/822048611/sys_ports_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/822048611/sys_ports_static.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/822048611/sys_ports_static.o.d" -o ${OBJECTDIR}/_ext/822048611/sys_ports_static.o ../src/system_config/default/framework/system/ports/src/sys_ports_static.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_init.o: ../src/system_config/default/system_init.c  .generated_files/flags/default/216a8b344424d0ef5b0dd0db241ab08685726eab .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_init.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_init.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_init.o ../src/system_config/default/system_init.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_interrupt.o: ../src/system_config/default/system_interrupt.c  .generated_files/flags/default/1c09c9e9b4a3c508e2b3cfaae458d4d1365523e4 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_interrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_interrupt.o ../src/system_config/default/system_interrupt.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_exceptions.o: ../src/system_config/default/system_exceptions.c  .generated_files/flags/default/309488413e50947498cf9ae56df896e3536ff9b0 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_exceptions.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_exceptions.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_exceptions.o ../src/system_config/default/system_exceptions.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1688732426/system_tasks.o: ../src/system_config/default/system_tasks.c  .generated_files/flags/default/2a9c65dacba7b67f2dc938cbe1f5b6d6b551bab4 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1688732426" 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1688732426/system_tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1688732426/system_tasks.o.d" -o ${OBJECTDIR}/_ext/1688732426/system_tasks.o ../src/system_config/default/system_tasks.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  .generated_files/flags/default/5a0c469d63cc5e7ae6243d091409760a11ab741 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/default/ea5b85615865149507d67471cbd760cf00e51008 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_VAR.o: ../src/PIC_VAR.c  .generated_files/flags/default/48e1849bcf8700182531a8391a5949ad2aef7b14 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_VAR.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_VAR.o ../src/PIC_VAR.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_MF40.o: ../src/PIC_MF40.c  .generated_files/flags/default/439a809191fbc583240efa4b15c65846deb6cc86 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_MF40.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_MF40.o ../src/PIC_MF40.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o: ../src/PIC_FT8X1_MF40.c  .generated_files/flags/default/99ad2a3d1f9029fb389cc680abef2d07b51a76c9 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_FT8X1_MF40.o ../src/PIC_FT8X1_MF40.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o: ../src/PIC_MODBUS.c  .generated_files/flags/default/fc5dcf71c53e2b0a043d350459b77d2095ab4765 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_MODBUS.o ../src/PIC_MODBUS.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/1360937237/PIC_Func.o: ../src/PIC_Func.c  .generated_files/flags/default/330bf583095c26a0ebb7ba82d5a0e2cd7fea0d1f .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_Func.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/PIC_Func.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/PIC_Func.o.d" -o ${OBJECTDIR}/_ext/1360937237/PIC_Func.o ../src/PIC_Func.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/249303043/sys_int_pic32.o: ../../../../microchip/harmony/v2_06/framework/system/int/src/sys_int_pic32.c  .generated_files/flags/default/9b5b77b9895eb77ed796516d39942e0d4f7fbed4 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/249303043" 
	@${RM} ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o.d 
	@${RM} ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/249303043/sys_int_pic32.o.d" -o ${OBJECTDIR}/_ext/249303043/sys_int_pic32.o ../../../../microchip/harmony/v2_06/framework/system/int/src/sys_int_pic32.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/_ext/496838821/sys_wdt.o: ../../../../microchip/harmony/v2_06/framework/system/wdt/src/sys_wdt.c  .generated_files/flags/default/681782368c5df907de12762239da83ef714ad697 .generated_files/flags/default/c792eedaa5eba2edcdabcdda85fe692db68ecc73
	@${MKDIR} "${OBJECTDIR}/_ext/496838821" 
	@${RM} ${OBJECTDIR}/_ext/496838821/sys_wdt.o.d 
	@${RM} ${OBJECTDIR}/_ext/496838821/sys_wdt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../../../../Microchip/harmony/v2_06/framework" -I"../src" -I"../src/system_config/default" -I"../src/default" -I"../../../../microchip/harmony/v2_06/framework" -I"../src/system_config/default/framework" -MP -MMD -MF "${OBJECTDIR}/_ext/496838821/sys_wdt.o.d" -o ${OBJECTDIR}/_ext/496838821/sys_wdt.o ../../../../microchip/harmony/v2_06/framework/system/wdt/src/sys_wdt.c    -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../microchip/harmony/v2_06/bin/framework/peripheral/PIC32MX350F256H_peripherals.a  
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\..\..\microchip\harmony\v2_06\bin\framework\peripheral\PIC32MX350F256H_peripherals.a      -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC02000:0x1FC02FEF -mreserve=boot@0x1FC02000:0x1FC0275F  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=4096,--defsym=_min_stack_size=2048,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../microchip/harmony/v2_06/bin/framework/peripheral/PIC32MX350F256H_peripherals.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\..\..\microchip\harmony\v2_06\bin\framework\peripheral\PIC32MX350F256H_peripherals.a      -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=4096,--defsym=_min_stack_size=2048,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml 
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/PIC-MF40.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
