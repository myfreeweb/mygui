/*!
	@file
	@author     Albert Semenov
	@date       08/2008
	@module
*/
#include "DemoKeeper.h"
#include "Base/Main.h"
#include "MyGUI_RTTLayer.h"
#include "MyGUI_RTTLayerNode.h"
#include "MyGUI_LayerNodeAnimation.h"
#include "WobbleNodeAnimator.h"
#include "FadeNodeAnimator.h"
#include "BaseLayout/BaseLayout.h"
#include "TreeControl.h"
#include "TreeControlItem.h"

	class SampleLayout : public wraps::BaseLayout
	{
	public:
		SampleLayout();

		void notifyTreeNodePrepare(MyGUI::TreeControl* pTreeControl, MyGUI::TreeControl::Node* pNode);
		MyGUI::UString getPath(MyGUI::TreeControl::Node* pNode) const;

	private:
		MyGUI::TreeControl * mpResourcesTree;
	};

SampleLayout * mSampleLayout;

    SampleLayout::SampleLayout() : BaseLayout("SampleLayout.layout")
    {
        assignWidget(mpResourcesTree, "ResourcesTree");
        mpResourcesTree->eventTreeNodePrepare = newDelegate(this, &SampleLayout::notifyTreeNodePrepare);

        MyGUI::TreeControl::Node* pRoot = mpResourcesTree->getRoot();
        Ogre::ArchiveManager::ArchiveMapIterator ArchiveIterator = Ogre::ArchiveManager::getSingleton().getArchiveIterator();
        while (ArchiveIterator.hasMoreElements())
        {
            Ogre::Archive* pArchive = ArchiveIterator.getNext();

            MyGUI::TreeControl::Node* pNode = new MyGUI::TreeControl::Node(pArchive->getName(), "Data");
            pNode->setData(pArchive);
            pRoot->add(pNode);
        }
    }

    void SampleLayout::notifyTreeNodePrepare(MyGUI::TreeControl* pTreeControl, MyGUI::TreeControl::Node* pNode)
    {
        if (pNode == pTreeControl->getRoot())
            return;

        pNode->removeAll();
        Ogre::Archive* pArchive = *(pNode->getData<Ogre::Archive*>());

        MyGUI::UString strPath(getPath(pNode));
        Ogre::StringVectorPtr Resources = pArchive->find(strPath + "*", false, true);
        for (Ogre::StringVector::iterator Iterator = Resources->begin(); Iterator != Resources->end(); Iterator++)
        {
            MyGUI::TreeControl::Node* pChild = new MyGUI::TreeControl::Node(*Iterator, "Folder");
            pChild->setData(pArchive);
            pNode->add(pChild);
        }

        Resources = pArchive->find(strPath + "*", false, false);
        for (Ogre::StringVector::iterator Iterator = Resources->begin(); Iterator != Resources->end(); Iterator++)
        {
            MyGUI::UString strName(*Iterator);
            MyGUI::UString strExtension;
            size_t nPosition = strName.rfind(".");
            if (nPosition != MyGUI::UString::npos)
            {
                strExtension = strName.substr(nPosition + 1);
                std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), tolower);
            }

            MyGUI::UString strImage;
            if (strExtension == "png" || strExtension == "tif" || strExtension == "tiff" || strExtension == "jpg" || strExtension == "jpeg")
                strImage = "Image";
            else
            if (strExtension == "mat" || strExtension == "material")
                strImage = "Material";
            else
            if (strExtension == "layout")
                strImage = "Layout";
            else
            if (strExtension == "ttf" || strExtension == "font" || strExtension == "fontdef")
                strImage = "Font";
            else
            if (strExtension == "txt" || strExtension == "text")
                strImage = "Text";
            else
            if (strExtension == "xml")
                strImage = "XML";
            else
            if (strExtension == "mesh")
                strImage = "Mesh";
            else
            if (strExtension == "htm" || strExtension == "html")
                strImage = "HTML";
            else
                strImage = "Unknown";

            MyGUI::TreeControl::Node* pChild = new MyGUI::TreeControl::Node(strName, strImage);
            pChild->setPrepared(true);
            pNode->add(pChild);
        }
    }

    MyGUI::UString SampleLayout::getPath(MyGUI::TreeControl::Node* pNode) const
    {
        if (!pNode || pNode == mpResourcesTree->getRoot())
            return MyGUI::UString();

        MyGUI::UString strPath;
        while (pNode->getParent() != mpResourcesTree->getRoot())
        {
            strPath = pNode->getText() + "/" + strPath;
            pNode = pNode->getParent();
        }

        return strPath;
    }

namespace demo
{

	DemoKeeper::DemoKeeper() :
		base::BaseManager()
	{
	}

	MyGUI::WindowPtr widget = nullptr;

	void notifyWindowButtonPressed(MyGUI::WindowPtr _sender, const std::string& _name)
	{
		if (_name == "close")
		{
			MyGUI::WidgetManager::getInstance().destroyWidget(_sender);
			widget = nullptr;
		}
		else if (_name == "check")
		{
			const MyGUI::IntCoord coord(0, 0, 1024, 768);
			const MyGUI::IntSize size(300, 300);

			if (widget->getCoord().width != coord.width)
			{
				widget->setCoord(coord);
			}
			else
			{
				widget->setCoord(coord.width / 2 - size.width / 2, coord.height / 2 - size.height / 2, size.width, size.height);
			}
		}
	}

	void test(int _value)
	{
	}

	void requestCreateWidgetItem(MyGUI::ItemBoxPtr _sender, MyGUI::WidgetPtr _item)
	{
		MyGUI::StaticTextPtr text = _item->createWidget<MyGUI::StaticText>("StaticText", MyGUI::IntCoord(0, 0, _item->getWidth(), _item->getHeight()), MyGUI::Align::Stretch);
		text->setNeedMouseFocus(false);
		_item->setUserData(text);
	}

	void requestCoordItem(MyGUI::ItemBoxPtr _sender, MyGUI::IntCoord& _coord, bool _drag)
	{
		_coord.set(0, 0, 100, 100);
	}

	void requestDrawItem(MyGUI::ItemBoxPtr _sender, MyGUI::WidgetPtr _item, const MyGUI::IBDrawItemInfo& _info)
	{
		MyGUI::StaticTextPtr text = *_item->getUserData<MyGUI::StaticTextPtr>();
		int data = *_sender->getItemDataAt<int>(_info.index);
		if (_info.drag)
		{
			text->setCaption(MyGUI::utility::toString(
				_info.drop_accept ? "#00FF00drag accept" : (_info.drop_refuse ? "#FF0000drag refuse" : "#0000FFdrag miss"),
				"\n#000000data : ", data));
		}
		else
		{
			text->setCaption(MyGUI::utility::toString(
				_info.drop_accept ? "#00FF00" : (_info.drop_refuse ? "#FF0000" : "#000000"), "index : ", _info.index,
				"\n#000000data : ", data,
				_info.active ? "\n#00FF00focus" : "\n#800000focus",
				_info.select ? "\n#00FF00select" : "\n#800000select"));
		}
	}

	void eventStartDrag(MyGUI::DDContainerPtr _sender, const MyGUI::DDItemInfo& _info, bool& _result)
	{
		MyGUI::ItemBoxPtr sender = _info.sender->castType<MyGUI::ItemBox>();
		int data = *sender->getItemDataAt<int>(_info.sender_index);
		_result = (data & 1) == 1;
	}

	void eventRequestDrop(MyGUI::DDContainerPtr _sender, const MyGUI::DDItemInfo& _info, bool& _result)
	{
		if (_info.receiver_index == MyGUI::ITEM_NONE)
		{
			_result = true;
		}
		else
		{
			MyGUI::ItemBoxPtr receiver = _info.receiver->castType<MyGUI::ItemBox>();
			int data = *receiver->getItemDataAt<int>(_info.receiver_index);
			_result = (data & 1) != 1;
		}
	}

	void eventDropResult(MyGUI::DDContainerPtr _sender, const MyGUI::DDItemInfo& _info, bool _result)
	{
		if (!_result) return;

		if (_info.receiver_index == MyGUI::ITEM_NONE)
		{
			MyGUI::ItemBoxPtr sender = _info.sender->castType<MyGUI::ItemBox>();
			MyGUI::ItemBoxPtr receiver = _info.receiver->castType<MyGUI::ItemBox>();
			int data = *sender->getItemDataAt<int>(_info.sender_index);
			sender->removeItemAt(_info.sender_index);
			receiver->addItem((int)data);
		}
		else
		{
			MyGUI::ItemBoxPtr sender = _info.sender->castType<MyGUI::ItemBox>();
			MyGUI::ItemBoxPtr receiver = _info.receiver->castType<MyGUI::ItemBox>();
			int data = *sender->getItemDataAt<int>(_info.sender_index);
			int data2 = *receiver->getItemDataAt<int>(_info.receiver_index);
			sender->removeItemAt(_info.sender_index);
			receiver->setItemDataAt(_info.receiver_index, data + data2);
		}
	}

	void init(MyGUI::Gui* _gui)
	{
		MyGUI::ItemBoxPtr box1 = _gui->createWidget<MyGUI::ItemBox>("ItemBoxV", MyGUI::IntCoord(10, 10, 300, 300), MyGUI::Align::Default, "Overlapped");
		box1->requestCreateWidgetItem = MyGUI::newDelegate(requestCreateWidgetItem);
		box1->requestCoordItem = MyGUI::newDelegate(requestCoordItem);
		box1->requestDrawItem = MyGUI::newDelegate(requestDrawItem);
		box1->eventStartDrag = MyGUI::newDelegate(eventStartDrag);
		box1->eventRequestDrop = MyGUI::newDelegate(eventRequestDrop);
		box1->eventDropResult = MyGUI::newDelegate(eventDropResult);

		box1->addItem((int)101);
		box1->addItem((int)43);
		box1->addItem((int)54);

		MyGUI::ItemBoxPtr box2 = _gui->createWidget<MyGUI::ItemBox>("ItemBoxV", MyGUI::IntCoord(410, 10, 300, 300), MyGUI::Align::Default, "Overlapped");
		box2->requestCreateWidgetItem = MyGUI::newDelegate(requestCreateWidgetItem);
		box2->requestCoordItem = MyGUI::newDelegate(requestCoordItem);
		box2->requestDrawItem = MyGUI::newDelegate(requestDrawItem);
		box2->eventStartDrag = MyGUI::newDelegate(eventStartDrag);
		box2->eventRequestDrop = MyGUI::newDelegate(eventRequestDrop);
		box2->eventDropResult = MyGUI::newDelegate(eventDropResult);

		box2->addItem((int)14);
		box2->addItem((int)273);
		box2->addItem((int)75);
	}

    void DemoKeeper::createScene()
    {

		//init(mGUI);
		MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
		factory.registryFactory<MyGUI::TreeControl>("Widget");
		factory.registryFactory<MyGUI::TreeControlItem>("Widget");

		mSampleLayout = new SampleLayout();
		/*MyGUI::FactoryManager::getInstance().registryFactory<WobbleNodeAnimator>("NodeAnimator");
		MyGUI::FactoryManager::getInstance().registryFactory<FadeNodeAnimator>("NodeAnimator");

		MyGUI::FactoryManager::getInstance().registryFactory<MyGUI::RTTLayer>("Layer");

		this->addResourceLocation("../../Media/UnitTests/TestApp");
		mGUI->load("test_layer.xml");

		widget = mGUI->createWidget<MyGUI::Window>("WindowCSMX", MyGUI::IntCoord(56, 16, 300, 300), MyGUI::Align::Default, "RTT_Test");
		widget->setCaption("Vertext mode");

		//MyGUI::EditPtr text = mGUI->createWidget<MyGUI::Edit>("EditStretch", MyGUI::IntCoord(356, 316, 300, 300), MyGUI::Align::Default, "RTT_Test");
		//text->setCaption("0 1 2 3");
		//text->setFontName("ManualFont");
		//text->setTextColour(MyGUI::Colour::White);

		//MyGUI::WidgetPtr widget2 = widget->createWidget<MyGUI::Window>("WindowCSX", MyGUI::IntCoord(46, 46, 164, 164), MyGUI::Align::Default, "RTT_Test");

		widget->eventWindowButtonPressed = MyGUI::newDelegate(notifyWindowButtonPressed);*/

	}

    void DemoKeeper::destroyScene()
    {
    }

	void setCornerData(MyGUI::VertexQuad& _quad, MyGUI::VertexQuad::Enum _corner, float _x, float _y, float _z, float _u, float _v, unsigned int _colour, bool _flipY)
	{
	}

	bool DemoKeeper::keyPressed( const OIS::KeyEvent &arg )
	{
		if (arg.key == OIS::KC_H)
		{
			widget = mGUI->createWidget<MyGUI::Window>("WindowCSMX", MyGUI::IntCoord(56, 16, 300, 300), MyGUI::Align::Default, "RTT_Test");
			widget->setCaption("Vertext mode");
			widget->eventWindowButtonPressed = MyGUI::newDelegate(notifyWindowButtonPressed);
		}

		if (widget == nullptr) return BaseManager::keyPressed( arg );

		if (arg.key == OIS::KC_1)
		{
			mCamera->setPolygonMode(Ogre::PM_SOLID);
		}
		else if (arg.key == OIS::KC_2)
		{
			mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
		}
		else if (arg.key == OIS::KC_3)
		{
			mCamera->setPolygonMode(Ogre::PM_POINTS);
		}
		else if (arg.key == OIS::KC_4)
		{
			MyGUI::LayerManager::EnumeratorLayer layer = MyGUI::LayerManager::getInstance().getEnumerator();
			while(layer.next())
			{
				if (layer->getName() == "RTT_Test")
				{
					//layer->castType<MyGUI::RTTLayer>()->setLayerNodeAnimation(&gCustomLayerNodeAnimation);
				}
			}
		}
		else if (arg.key == OIS::KC_SPACE)
		{
			MyGUI::LayerManager::EnumeratorLayer layer = MyGUI::LayerManager::getInstance().getEnumerator();
			while(layer.next())
			{
				if (layer->getName() == "RTT_Test")
				{
					MyGUI::EnumeratorILayerNode node = layer->getEnumerator();
					while(node.next())
					{
						MyGUI::RTTLayerNode* rttnode = node->castType<MyGUI::RTTLayerNode>(false);
						if (rttnode != nullptr)
						{
							rttnode->setCacheUsing(!rttnode->getCacheUsing());

							if (rttnode->getCacheUsing())
							{
								/*if (rttnode->getLayerNodeAnimation() != nullptr)
								{
									//rttnode->setLayerNodeAnimation(nullptr);
									widget->setCaption("RTT mode");
								}
								else
								{
									//rttnode->setLayerNodeAnimation(&gCustomLayerNodeAnimation);
									widget->setCaption("Abstract mode");
								}*/
							}
							else
							{
								widget->setCaption("Vertext mode");
							}

						}
					}
				}
			}
		}

		return BaseManager::keyPressed( arg );
	}

} // namespace demo

MYGUI_APP(demo::DemoKeeper)
