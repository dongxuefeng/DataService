#include <tinystr.h>
#include <tinyxml.h>
#ifndef _xmlfunc_h__
#define _xmlfunc_h__

/*!
*  \brief 获取xml文件的声明。
*
*  \param XmlFile xml文件全路径。
*  \param strVersion  Version属性值
*  \param strStandalone Standalone属性值
*  \param strEncoding Encoding属性值
*  \return 是否成功。true为成功，false表示失败。
*/
bool GetXmlDeclare(std::string XmlFile,
                   std::string &strVersion,
                   std::string &strStandalone,
                   std::string &strEncoding)
{
    // 定义一个TiXmlDocument类指针
    TiXmlDocument *pDoc = new TiXmlDocument();
    if (NULL==pDoc)
    {
        return false;
    }
    pDoc->LoadFile(XmlFile);
      TiXmlNode* pXmlFirst = pDoc->FirstChild();
      if (NULL != pXmlFirst)
     {
          TiXmlDeclaration* pXmlDec = pXmlFirst->ToDeclaration();
          if (NULL != pXmlDec)
          {
              strVersion = pXmlDec->Version();
              strStandalone = pXmlDec->Standalone();
              strEncoding = pXmlDec->Encoding();
          }
      }
      return true;
}
/*!
*  \brief 获取xml文件的声明。
*
*  \param XmlFile xml文件全路径。
*  \param strVersion  Version属性值
*  \param strStandalone Standalone属性值
*  \param strEncoding Encoding属性值
*  \return 是否成功。true为成功，false表示失败。
*/
bool GetXmlDeclare(std::string XmlFile,
       std::string &strVersion,
       std::string &strStandalone,
       std::string &strEncoding)
{
 // 定义一个TiXmlDocument类指针
 TiXmlDocument *pDoc = new TiXmlDocument();
 if (NULL==pDoc)
 {
  return false;
 }
 pDoc->LoadFile(XmlFile);
   TiXmlNode* pXmlFirst = pDoc->FirstChild();
   if (NULL != pXmlFirst)
     {
          TiXmlDeclaration* pXmlDec = pXmlFirst->ToDeclaration();
          if (NULL != pXmlDec)
          {
              strVersion = pXmlDec->Version();
              strStandalone = pXmlDec->Standalone();
              strEncoding = pXmlDec->Encoding();
       }
   }
   return true;
}

//      我们发现无论查询节点、删除节点、
//     修改节点和增加节点，其实都离不开一个函数，
//     就是根据节点名获取相关节点指针。
//     那么我们就先实现一个根据节点名获取节点指针的函数：
/*!
*  \brief 通过根节点和节点名获取节点指针。
*
*  \param pRootEle   xml文件的根节点。
*  \param strNodeName  要查询的节点名
*  \param Node      需要查询的节点指针
*  \return 是否找到。true为找到相应节点指针，false表示没有找到相应节点指针。
*/
bool GetNodePointerByName(TiXmlElement* pRootEle,std::string &strNodeName,TiXmlElement* &Node)
{
     // 假如等于根节点名，就退出
     if (strNodeName==pRootEle->Value())
     {
         Node = pRootEle;
         return true;
     }
      TiXmlElement* pEle = pRootEle;
      for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
    {
          //递归处理子节点，获取节点指针
          if(GetNodePointerByName(pEle,strNodeName,Node))
              return true;
     }
     return false;
}
/*!
*  \brief 通过根节点和节点名获取节点指针。
*
*  \param pRootEle   xml文件的根节点。
*  \param strNodeName  要查询的节点名
*  \param Node      需要查询的节点指针
*  \return 是否找到。true为找到相应节点指针，false表示没有找到相应节点指针。
*/
bool GetNodePointerByName(TiXmlElement* pRootEle,std::string &strNodeName,TiXmlElement* &Node)
{
  // 假如等于根节点名，就退出
     if (strNodeName==pRootEle->Value())
     {
         Node = pRootEle;
   return true;
     }
   TiXmlElement* pEle = pRootEle;
      for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
    {
          //递归处理子节点，获取节点指针
          if(GetNodePointerByName(pEle,strNodeName,Node))
     return true;
     }
  return false;
}
//       有了这个函数，我们就很容易实现查询节点的相应文本或属性值。
view plaincopy to clipboardprint?
  /*!
*  \brief 通过节点查询。
*
*  \param XmlFile   xml文件全路径。
*  \param strNodeName  要查询的节点名
*  \param strText      要查询的节点文本
*  \return 是否成功。true为成功，false表示失败。
*/
bool QueryNode_Text(std::string XmlFile,std::string strNodeName,std::string &strText)
{
    // 定义一个TiXmlDocument类指针
    TiXmlDocument *pDoc = new TiXmlDocument();
    if (NULL==pDoc)
    {
        return false;
    }
    pDoc->LoadFile(XmlFile);
    TiXmlElement *pRootEle = pDoc->RootElement();
    if (NULL==pRootEle)
    {
        return false;
    }
   TiXmlElement *pNode = NULL;
   GetNodePointerByName(pRootEle,strNodeName,pNode);
   if (NULL!=pNode)
   {
        strText = pNode->GetText();
        return true;
   }
   else
   {
        return false;
   }

}
/*!
*  \brief 通过节点查询。
*
*  \param XmlFile   xml文件全路径。
*  \param strNodeName  要查询的节点名
*  \param AttMap      要查询的属性值，这是一个map，前一个为属性名，后一个为属性值
*  \return 是否成功。true为成功，false表示失败。
*/
bool QueryNode_Attribute(std::string XmlFile,std::string strNodeName,std::map &AttMap)
{
    // 定义一个TiXmlDocument类指针
    typedef std::pair String_Pair;
    TiXmlDocument *pDoc = new TiXmlDocument();
    if (NULL==pDoc)
    {
        return false;
    }
    pDoc->LoadFile(XmlFile);
    TiXmlElement *pRootEle = pDoc->RootElement();
    if (NULL==pRootEle)
    {
        return false;
    }
    TiXmlElement *pNode = NULL;
    GetNodePointerByName(pRootEle,strNodeName,pNode);
    if (NULL!=pNode)
    {
        TiXmlAttribute* pAttr = NULL;
        for (pAttr = pNode->FirstAttribute(); pAttr; pAttr = pAttr->Next())
        {
            std::string strAttName = pAttr->Name();
            std::string strAttValue = pAttr->Value();
            AttMap.insert(String_Pair(strAttName,strAttValue));
        }
        return true;
    }
    else
    {
        return false;
    }
    return true;
}
  /*!
*  \brief 通过节点查询。
*
*  \param XmlFile   xml文件全路径。
*  \param strNodeName  要查询的节点名
*  \param strText      要查询的节点文本
*  \return 是否成功。true为成功，false表示失败。
*/
bool QueryNode_Text(std::string XmlFile,std::string strNodeName,std::string &strText)
{
 // 定义一个TiXmlDocument类指针
 TiXmlDocument *pDoc = new TiXmlDocument();
 if (NULL==pDoc)
 {
  return false;
 }
 pDoc->LoadFile(XmlFile);
 TiXmlElement *pRootEle = pDoc->RootElement();
 if (NULL==pRootEle)
 {
  return false;
 }
   TiXmlElement *pNode = NULL;
   GetNodePointerByName(pRootEle,strNodeName,pNode);
   if (NULL!=pNode)
   {
        strText = pNode->GetText();
  return true;
   }
   else
   {
     return false;
   }

}
/*!
*  \brief 通过节点查询。
*
*  \param XmlFile   xml文件全路径。
*  \param strNodeName  要查询的节点名
*  \param AttMap      要查询的属性值，这是一个map，前一个为属性名，后一个为属性值
*  \return 是否成功。true为成功，false表示失败。
*/
bool QueryNode_Attribute(std::string XmlFile,std::string strNodeName,std::map &AttMap)
{
 // 定义一个TiXmlDocument类指针
    typedef std::pair String_Pair;
 TiXmlDocument *pDoc = new TiXmlDocument();
 if (NULL==pDoc)
 {
  return false;
 }
 pDoc->LoadFile(XmlFile);
 TiXmlElement *pRootEle = pDoc->RootElement();
 if (NULL==pRootEle)
 {
  return false;
 }
 TiXmlElement *pNode = NULL;
 GetNodePointerByName(pRootEle,strNodeName,pNode);
 if (NULL!=pNode)
 {
  TiXmlAttribute* pAttr = NULL;
  for (pAttr = pNode->FirstAttribute(); pAttr; pAttr = pAttr->Next())
  {
   std::string strAttName = pAttr->Name();
   std::string strAttValue = pAttr->Value();
   AttMap.insert(String_Pair(strAttName,strAttValue));
  }
  return true;
 }
 else
 {
  return false;
 }
 return true;
}
   下面是删除指定节点的函数，其中考虑了删除根节点的情况:
view plaincopy to clipboardprint?
/*!
*  \brief 删除指定节点的值。
*
*  \param XmlFile xml文件全路径。
*  \param strNodeName 指定的节点名。
*  \return 是否成功。true为成功，false表示失败。
*/
bool DelNode(std::string XmlFile,std::string strNodeName)
{
    // 定义一个TiXmlDocument类指针
    TiXmlDocument *pDoc = new TiXmlDocument();
    if (NULL==pDoc)
    {
        return false;
    }
    pDoc->LoadFile(XmlFile);
    TiXmlElement *pRootEle = pDoc->RootElement();
    if (NULL==pRootEle)
    {
        return false;
    }
    TiXmlElement *pNode = NULL;
    GetNodePointerByName(pRootEle,strNodeName,pNode);
    // 假如是根节点
    if (pRootEle==pNode)
    {
          if(pDoc->RemoveChild(pRootEle))
          {
               pDoc->SaveFile(XmlFile);
               return true;
          }
          else
              return false;
    }
    // 假如是其它节点
    if (NULL!=pNode)
    {
        TiXmlNode *pParNode =  pNode->Parent();
        if (NULL==pParNode)
        {
               return false;
        }

        TiXmlElement* pParentEle = pParNode->ToElement();
        if (NULL!=pParentEle)
        {
            if(pParentEle->RemoveChild(pNode))
                 pDoc->SaveFile(XmlFile);
            else
                return false;
        }
    }
    else
    {
          return false;
    }
     return false;
}

#endif
