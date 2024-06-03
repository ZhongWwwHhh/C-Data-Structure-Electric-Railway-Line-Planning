const searchWay = async (startStation, endStation) => {
    return await window.apiSearch.search(startStation, endStation)
}

// 地区/线路/站点选项
const updateSubwayData = () => {
    var subwayDataSelect = document.getElementById('subwayData');
    var selectedSubwayValue = subwayDataSelect.options[subwayDataSelect.selectedIndex].value;
    var selectedSubwayData = JSON.parse(selectedSubwayValue);
    // 替换地铁图片
    var map = document.getElementById('map')
    var subwayMap = selectedSubwayData[0];
    map.src = `./${subwayMap}`;
    // 图片缩放还原
    map.style.cssText = `
        -webkit-app-region: no-drag;
        position: absolute;
        width: 100%;
        height: 100%;
        object-fit: contain;
        left: 0;
        top: 0;
    `;
    // 替换站点数据
    var subwayDataFile = './Data/' + selectedSubwayData[1];
    // core更新
    window.apiSearch.readData(subwayDataFile)
        .then(data => {
            console.log(data);
        })
        .catch(error => {
            window.apiError.setError(error);
        });
    // 图形选项
    fetch(subwayDataFile)
        .then(response => response.json())
        .then(data => {
            const line1Select = document.getElementById('line1');
            const line2Select = document.getElementById('line2');
            // 清空原有选项
            line1Select.innerHTML = '<option value="">--请选择地铁线--</option>';
            line2Select.innerHTML = '<option value="">--请选择地铁线--</option>';
            line1Select.selectedIndex = 0;
            line2Select.selectedIndex = 0;
            const startStationSelect = document.getElementById('startStation');
            startStationSelect.innerHTML = '<option value="">--请选择站点--</option>';
            startStationSelect.selectedIndex = 0;
            const endStationSelect = document.getElementById('endStation');
            endStationSelect.innerHTML = '<option value="">--请选择站点--</option>';
            endStationSelect.selectedIndex = 0;
            // 写入线路选项
            for (const lineId in data.lines) {
                const lineName = data.lines[lineId];
                const option1 = document.createElement('option');
                const option2 = document.createElement('option');
                option1.value = lineId;
                option2.value = lineId;
                option1.textContent = lineName;
                option2.textContent = lineName;
                line1Select.appendChild(option1);
                line2Select.appendChild(option2);
            }

            line1Select.addEventListener('change', function () {
                const selectedLine = line1Select.value;
                const startStationSelect = document.getElementById('startStation');
                startStationSelect.innerHTML = '<option value="">--请选择站点--</option>';
                startStationSelect.selectedIndex = 0;
                for (const siteId in data.sites) {
                    if (siteId.startsWith(selectedLine)) {
                        const stationName = data.sites[siteId];
                        const option = document.createElement('option');
                        option.value = siteId;
                        option.textContent = stationName;
                        startStationSelect.appendChild(option);
                    }
                }
            });

            line2Select.addEventListener('change', function () {
                const selectedLine = line2Select.value;
                const endStationSelect = document.getElementById('endStation');
                endStationSelect.innerHTML = '<option value="">--请选择站点--</option>';
                endStationSelect.selectedIndex = 0;
                for (const siteId in data.sites) {
                    if (siteId.startsWith(selectedLine)) {
                        const stationName = data.sites[siteId];
                        const option = document.createElement('option');
                        option.value = siteId;
                        option.textContent = stationName;
                        endStationSelect.appendChild(option);
                    }
                }
            });
        });
}
// 切换地区时更新
document.getElementById('subwayData').addEventListener('change', updateSubwayData);

window.onload = function () {
    // 获取可用的地铁图
    window.apiSearch.listSubway()
        .then(list => {
            console.log(list);
            var subwayDataSelect = document.getElementById('subwayData');
            for (var key in list) {
                var option = document.createElement('option');
                option.text = key;
                option.value = JSON.stringify(list[key]);
                subwayDataSelect.add(option);
            }
            subwayDataSelect.selectedIndex = 0;
            updateSubwayData();
        })
        .catch(error => {
            window.apiError.setError(error);
        });

    // 获取 HTML 元素
    var startStationSelect = document.getElementById('startStation');
    var endStationSelect = document.getElementById('endStation');
    var searchButton = document.getElementById('search');
    var resultDiv = document.getElementById('result');

    // 监听 click 事件
    searchButton.addEventListener('click', function () {
        // 获取起始站和终点站
        var startStation = startStationSelect.value;
        var endStation = endStationSelect.value;
        // 判断是否选择了起始站和终点站
        if (startStation === "" || endStation === "") {
            resultDiv.textContent = "请选择始末站点";
            resultDiv.style.display = "block";
            return;
        }
        searchWay(startStation, endStation)
            .then(result => {
                // 将结果写入 resultDiv
                resultDiv.innerHTML = result;
                resultDiv.style.display = "block";
            })
            .catch(error => {
                window.apiError.setError(error);
            });
    });
};