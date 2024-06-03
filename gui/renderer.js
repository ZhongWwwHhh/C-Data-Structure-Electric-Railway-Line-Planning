const searchWay = async (startStation, endStation) => {
    return await window.apiSearch.search(startStation, endStation)
}

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
                resultDiv.textContent = result;
                resultDiv.style.display = "block";
            })
            .catch(error => {
                window.apiError.setError(error);
            });
    });
};