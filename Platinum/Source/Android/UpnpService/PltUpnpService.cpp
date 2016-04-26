/*****************************************************************
|
|   Platinum - Light Sample Device
|
| Copyright (c) 2004-2010, Plutinosoft, LLC.
| All rights reserved.
| http://www.plutinosoft.com
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or (at your option) any later version.
|
| OEMs, ISVs, VARs and other distributors that combine and 
| distribute commercially licensed software with Platinum software
| and do not wish to distribute the source code for the commercially
| licensed software under version 2, or (at your option) any later
| version, of the GNU General Public License (the "GPL") must enter
| into a commercial license agreement with Plutinosoft, LLC.
| licensing@plutinosoft.com
| 
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; see the file LICENSE.txt. If not, write to
| the Free Software Foundation, Inc., 
| 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
| http://www.gnu.org/licenses/gpl-2.0.html
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <stdlib.h>
#include "NptTypes.h"
#include "Platinum_Upnp.h"
#include "PltUpnpService.h"
#include <android/log.h>

NPT_SET_LOCAL_LOGGER("platinum.upnpservice")

/*----------------------------------------------------------------------
|   forward references
+---------------------------------------------------------------------*/
extern const char* SCPDXML;

/*----------------------------------------------------------------------
|   PLT_LightSampleDevice::PLT_LightSampleDevice
+---------------------------------------------------------------------*/
PLT_UpnpServiceDevice::PLT_UpnpServiceDevice(const char* FriendlyName, const char* UUID) :	
    PLT_DeviceHost("/", UUID, "urn:schemas-upnp-org:device:UpnpService:1", FriendlyName)
{
}

/*----------------------------------------------------------------------
|   PLT_LightSampleDevice::~PLT_LightSampleDevice
+---------------------------------------------------------------------*/
PLT_UpnpServiceDevice::~PLT_UpnpServiceDevice()
{
}

/*----------------------------------------------------------------------
|   PLT_LightSampleDevice::SetupServices
+---------------------------------------------------------------------*/
NPT_Result
PLT_UpnpServiceDevice::SetupServices()
{
    NPT_Result res;
    PLT_Service* service = new PLT_Service(
        this,
        "urn:schemas-upnp-org:service:UpnpService:1", 
        "urn:upnp-org:serviceId:UpnpService.001",
        "UpnpService");
    NPT_CHECK_LABEL_FATAL(res = service->SetSCPDXML((const char*)SCPDXML), failure);
    NPT_CHECK_LABEL_FATAL(res = AddService(service), failure);

    //service->SetStateVariable("Status", "True");

    return NPT_SUCCESS;
    
failure:
    delete service;
    return res;
}

/*----------------------------------------------------------------------
|   PLT_LightSampleDevice::OnAction
+---------------------------------------------------------------------*/
NPT_Result
PLT_UpnpServiceDevice::OnAction(PLT_ActionReference&          action,
                                const PLT_HttpRequestContext& context)
{
    NPT_COMPILER_UNUSED(context);

    /* parse the action name */
    NPT_String name = action->GetActionDesc().GetName();
   
    if (name.Compare("Account") == 0){
        NPT_String auth;
        action->GetArgumentValue("auth", auth);        

        if(auth.Compare("www.windaka.com") != 0 ) {
                action->SetError(501, "Auth Failed");
            return NPT_FAILURE;
        }            
        NPT_String userName, password, domain,autocall;
        action->GetArgumentValue("userName", userName);
        action->GetArgumentValue("password", password);
        action->GetArgumentValue("domain", domain);
        action->GetArgumentValue("autocall", autocall);
        int status;
        // 清除账户信息
        status = system("/system/bin/sh -c \"am broadcast -a org.linphone.remote.ConfigBroadcastReceiver_ResetAccount\"");
        __android_log_print(ANDROID_LOG_DEBUG, "UpnpService", "cmd: %s\t error: %d"," /system/bin/sh -c \"am broadcast -a org.linphone.remote.ConfigBroadcastReceiver_ResetAccount\"", status);

        //设置账户信息
        status = system("/system/bin/sh -c \"am broadcast -a org.linphone.remote.ConfigBroadcastReceiver -e userName \""+userName+"\" -e passwd \""+password+"\" -e domain \""+domain+"\"\"");
         __android_log_print(ANDROID_LOG_DEBUG, "UpnpService", "cmd:/system/bin/sh -c \"am broadcast -a org.linphone.remote.ConfigBroadcastReceiver -e userName \""+userName+"\" -e passwd \""+password+"\" -e domain \""+domain+"\"\"\t error: %d", status);

        //设置呼叫账户
        status = system("/system/bin/sh -c \"am broadcast -a org.linphone.remote.ConfigBroadcastReceiver_AutoCallSipAddress -e sipAddress \""+autocall+"\"\"");
        __android_log_print(ANDROID_LOG_DEBUG, "UpnpService", "cmd: /system/bin/sh -c \"am broadcast -a org.linphone.remote.ConfigBroadcastReceiver_AutoCallSipAddress -e sipAddress \""+autocall+"\"\"\t error: %d", status);
        return NPT_SUCCESS;
    }
    
    action->SetError(501, "Action Failed");
    return NPT_FAILURE;
}
const char* SCPDXML =
"<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">"
"   <specVersion>"
"       <major>1</major>"
"       <minor>0</minor>"
"   </specVersion>"
"   <actionList>"
"       <action>"
"           <name>Account</name>"
"           <argumentList>"
"               <argument>"
"                   <name>auth</name>"
"                   <direction>in</direction>"
"                   <relatedStateVariable>Target</relatedStateVariable>"
"               </argument>"
"               <argument>"
"                   <name>userName</name>"
"                   <direction>in</direction>"
"                   <relatedStateVariable>Target</relatedStateVariable>"
"               </argument>"
"               <argument>"
"                   <name>password</name>"
"                   <direction>in</direction>"
"                   <relatedStateVariable>Target</relatedStateVariable>"
"               </argument>"
"               <argument>"
"                   <name>domain</name>"
"                   <direction>in</direction>"
"                   <relatedStateVariable>Target</relatedStateVariable>"
"               </argument>"
"               <argument>"
"                   <name>autocall</name>"
"                   <direction>in</direction>"
"                   <relatedStateVariable>Target</relatedStateVariable>"
"               </argument>"
"       </argumentList>"
"   </action>"
"   </actionList>"
"   <serviceStateTable>"
"       <stateVariable sendEvents=\"no\">"
"           <name>Target</name>"
"           <dataType>string</dataType>"
"       </stateVariable>"
"   </serviceStateTable>"
"</scpd>";
