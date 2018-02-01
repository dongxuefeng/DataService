#include <tinystr.h>
#include <tinyxml.h>
#ifndef _xmlfunc_h__
#define _xmlfunc_h__

/*!
*  \brief ��ȡxml�ļ���������
*
*  \param XmlFile xml�ļ�ȫ·����
*  \param strVersion  Version����ֵ
*  \param strStandalone Standalone����ֵ
*  \param strEncoding Encoding����ֵ
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
*/
bool GetXmlDeclare(std::string XmlFile,
                   std::string &strVersion,
                   std::string &strStandalone,
                   std::string &strEncoding)
{
    // ����һ��TiXmlDocument��ָ��
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
*  \brief ��ȡxml�ļ���������
*
*  \param XmlFile xml�ļ�ȫ·����
*  \param strVersion  Version����ֵ
*  \param strStandalone Standalone����ֵ
*  \param strEncoding Encoding����ֵ
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
*/
bool GetXmlDeclare(std::string XmlFile,
       std::string &strVersion,
       std::string &strStandalone,
       std::string &strEncoding)
{
 // ����һ��TiXmlDocument��ָ��
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

//      ���Ƿ������۲�ѯ�ڵ㡢ɾ���ڵ㡢
//     �޸Ľڵ�����ӽڵ㣬��ʵ���벻��һ��������
//     ���Ǹ��ݽڵ�����ȡ��ؽڵ�ָ�롣
//     ��ô���Ǿ���ʵ��һ�����ݽڵ�����ȡ�ڵ�ָ��ĺ�����
/*!
*  \brief ͨ�����ڵ�ͽڵ�����ȡ�ڵ�ָ�롣
*
*  \param pRootEle   xml�ļ��ĸ��ڵ㡣
*  \param strNodeName  Ҫ��ѯ�Ľڵ���
*  \param Node      ��Ҫ��ѯ�Ľڵ�ָ��
*  \return �Ƿ��ҵ���trueΪ�ҵ���Ӧ�ڵ�ָ�룬false��ʾû���ҵ���Ӧ�ڵ�ָ�롣
*/
bool GetNodePointerByName(TiXmlElement* pRootEle,std::string &strNodeName,TiXmlElement* &Node)
{
     // ������ڸ��ڵ��������˳�
     if (strNodeName==pRootEle->Value())
     {
         Node = pRootEle;
         return true;
     }
      TiXmlElement* pEle = pRootEle;
      for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
    {
          //�ݹ鴦���ӽڵ㣬��ȡ�ڵ�ָ��
          if(GetNodePointerByName(pEle,strNodeName,Node))
              return true;
     }
     return false;
}
/*!
*  \brief ͨ�����ڵ�ͽڵ�����ȡ�ڵ�ָ�롣
*
*  \param pRootEle   xml�ļ��ĸ��ڵ㡣
*  \param strNodeName  Ҫ��ѯ�Ľڵ���
*  \param Node      ��Ҫ��ѯ�Ľڵ�ָ��
*  \return �Ƿ��ҵ���trueΪ�ҵ���Ӧ�ڵ�ָ�룬false��ʾû���ҵ���Ӧ�ڵ�ָ�롣
*/
bool GetNodePointerByName(TiXmlElement* pRootEle,std::string &strNodeName,TiXmlElement* &Node)
{
  // ������ڸ��ڵ��������˳�
     if (strNodeName==pRootEle->Value())
     {
         Node = pRootEle;
   return true;
     }
   TiXmlElement* pEle = pRootEle;
      for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
    {
          //�ݹ鴦���ӽڵ㣬��ȡ�ڵ�ָ��
          if(GetNodePointerByName(pEle,strNodeName,Node))
     return true;
     }
  return false;
}
//       ����������������Ǿͺ�����ʵ�ֲ�ѯ�ڵ����Ӧ�ı�������ֵ��
view plaincopy to clipboardprint?
  /*!
*  \brief ͨ���ڵ��ѯ��
*
*  \param XmlFile   xml�ļ�ȫ·����
*  \param strNodeName  Ҫ��ѯ�Ľڵ���
*  \param strText      Ҫ��ѯ�Ľڵ��ı�
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
*/
bool QueryNode_Text(std::string XmlFile,std::string strNodeName,std::string &strText)
{
    // ����һ��TiXmlDocument��ָ��
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
*  \brief ͨ���ڵ��ѯ��
*
*  \param XmlFile   xml�ļ�ȫ·����
*  \param strNodeName  Ҫ��ѯ�Ľڵ���
*  \param AttMap      Ҫ��ѯ������ֵ������һ��map��ǰһ��Ϊ����������һ��Ϊ����ֵ
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
*/
bool QueryNode_Attribute(std::string XmlFile,std::string strNodeName,std::map &AttMap)
{
    // ����һ��TiXmlDocument��ָ��
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
*  \brief ͨ���ڵ��ѯ��
*
*  \param XmlFile   xml�ļ�ȫ·����
*  \param strNodeName  Ҫ��ѯ�Ľڵ���
*  \param strText      Ҫ��ѯ�Ľڵ��ı�
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
*/
bool QueryNode_Text(std::string XmlFile,std::string strNodeName,std::string &strText)
{
 // ����һ��TiXmlDocument��ָ��
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
*  \brief ͨ���ڵ��ѯ��
*
*  \param XmlFile   xml�ļ�ȫ·����
*  \param strNodeName  Ҫ��ѯ�Ľڵ���
*  \param AttMap      Ҫ��ѯ������ֵ������һ��map��ǰһ��Ϊ����������һ��Ϊ����ֵ
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
*/
bool QueryNode_Attribute(std::string XmlFile,std::string strNodeName,std::map &AttMap)
{
 // ����һ��TiXmlDocument��ָ��
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
   ������ɾ��ָ���ڵ�ĺ��������п�����ɾ�����ڵ�����:
view plaincopy to clipboardprint?
/*!
*  \brief ɾ��ָ���ڵ��ֵ��
*
*  \param XmlFile xml�ļ�ȫ·����
*  \param strNodeName ָ���Ľڵ�����
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
*/
bool DelNode(std::string XmlFile,std::string strNodeName)
{
    // ����һ��TiXmlDocument��ָ��
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
    // �����Ǹ��ڵ�
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
    // �����������ڵ�
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
