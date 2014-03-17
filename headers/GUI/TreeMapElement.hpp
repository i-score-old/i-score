/*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 * iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
 *
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */

#ifndef TREEMAPELEMENT_HPP_
#define TREEMAPELEMENT_HPP_

#include <QBoxLayout>
#include <QWidget>
#include <QLabel>
#include <vector>
#include <string>
#include <map>

class TreeMap;

enum ElementType { Node, Leave, Attribute };

class TreeMapElement : public QWidget {
  Q_OBJECT

  public:
    TreeMapElement(QWidget *parent = 0);

    void setAttributes(TreeMapElement *parentElt, const std::string &message, ElementType type);

    ElementType
    type() { return _type; }

    std::string
    value() { return _value; }

    void setValue(const std::string &value);

    void setSelected(bool selected);

    bool
    selected() { return _selected; }

    std::string
    message() { return _message; }

    unsigned int
    descendance() { return _descendanceCount; }

    std::string address();

    TreeMapElement* findChild(const std::string &message);
    TreeMapElement* addChild(const std::string &message, ElementType type);
    void addChildren(const std::vector<std::string>& nodes, const std::vector<std::string>& leaves,
                     const std::vector<std::string>& attributes, const std::vector<std::string>& attributesValue);

    std::map<std::string, TreeMapElement*> children() { return _children; }

    std::string upAddress();

  signals:
    void oneMoreChild();
    void familyExpanded();

  public slots:
    void increaseDescendance();
    void childAdded();

  private:
    void setGlobalTreeMap(TreeMap *treeMap);
    void addChild(TreeMapElement* child);
    unsigned int _ID;
    ElementType _type;
    TreeMapElement* _parent;
    std::string _message;
    std::string _value;
    std::map<std::string, TreeMapElement*> _children;
    std::vector<TreeMapElement*> _nodes;
    std::vector<TreeMapElement*> _leaves;
    std::vector<TreeMapElement*> _attributes;
    QBoxLayout *_layout;
    QVBoxLayout *_globalLayout;
    QLabel *_titleLabel;
    unsigned int _descendanceCount;
    bool _selected;
    static TreeMap *TREE_MAP;

  protected:
    QBoxLayout::Direction direction();
    void paintEvent(QPaintEvent * event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    friend class TreeMap; /// \todo vérifier l'implication de friend class (par jaime Chao)
};
#endif /* TREEMAPELEMENT_HPP_ */
